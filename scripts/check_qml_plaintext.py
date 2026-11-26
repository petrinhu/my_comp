#!/usr/bin/env python3
"""Gate de texto puro no QML (MC-10).

Text e Label usam Text.AutoText por padrão: uma string que comece com marcação
("<b>", "<img src=...>", "<font size=...>") é desenhada como rich text. Nomes de
processo (cmdline), mensagens do journal e nomes de dispositivos Bluetooth vêm de
fora e podem falsificar a interface (por exemplo, o nome no diálogo de matar processo).

Regra: todo Text/Label cujo `text:` não seja um literal puro ("..." ou qsTr("...")
sem .arg) precisa declarar `textFormat:` explicitamente. Use Text.PlainText para
dados externos; StyledText/RichText só com conteúdo controlado pelo app.

Uso: scripts/check_qml_plaintext.py [diretório]   (padrão: src/presentation/qml)
Saída 0 sem achados; 1 com achados (um por linha, arquivo:linha).
"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ELEMENT_RE = re.compile(r"\b(Text|Label)\s*\{")
TEXT_PROP_RE = re.compile(r"(?<![\w.])text\s*:")
LITERAL_RE = re.compile(r'^(qsTr\(\s*"[^"\\]*"\s*\)|"[^"\\]*")$')


def matching_brace(src: str, open_idx: int) -> int:
    """Índice do '}' que fecha o '{' em open_idx, ignorando chaves dentro de strings."""
    depth = 0
    quote = ""
    i = open_idx
    while i < len(src):
        c = src[i]
        if quote:
            if c == "\\":
                i += 2
                continue
            if c == quote:
                quote = ""
        elif c in "\"'":
            quote = c
        elif c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return len(src) - 1


def top_level(body: str) -> str:
    """Remove os blocos filhos ({...}) do corpo, mantendo só as propriedades do elemento."""
    out = []
    depth = 0
    quote = ""
    for c in body:
        if quote:
            if depth == 0:
                out.append(c)
            if c == quote:
                quote = ""
            continue
        if c in "\"'":
            quote = c
            if depth == 0:
                out.append(c)
            continue
        if c == "{":
            depth += 1
            continue
        if c == "}":
            depth -= 1
            continue
        if depth == 0:
            out.append(c)
    return "".join(out)


def text_expression(props: str) -> str | None:
    m = TEXT_PROP_RE.search(props)
    if not m:
        return None
    rest = props[m.end():]
    end = len(rest)
    for sep in (";", "\n"):
        pos = rest.find(sep)
        if pos != -1:
            end = min(end, pos)
    return rest[:end].strip()


def check_file(path: Path) -> list[str]:
    src = path.read_text(encoding="utf-8")
    findings = []
    for m in ELEMENT_RE.finditer(src):
        open_idx = m.end() - 1
        close_idx = matching_brace(src, open_idx)
        props = top_level(src[open_idx + 1:close_idx])
        expr = text_expression(props)
        if expr is None or LITERAL_RE.match(expr):
            continue
        if re.search(r"(?<![\w.])textFormat\s*:", props):
            continue
        line = src.count("\n", 0, m.start()) + 1
        findings.append(f"{path}:{line}: {m.group(1)} com text dinâmico sem textFormat ({expr[:60]})")
    return findings


def main(argv: list[str]) -> int:
    root = Path(argv[1]) if len(argv) > 1 else Path(__file__).resolve().parent.parent / "src/presentation/qml"
    findings = []
    for qml in sorted(root.rglob("*.qml")):
        findings.extend(check_file(qml))
    for f in findings:
        print(f)
    if findings:
        print(f"\n{len(findings)} Text/Label sem textFormat. Use textFormat: Text.PlainText em dado externo (MC-10).")
        return 1
    print("check_qml_plaintext: OK")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
