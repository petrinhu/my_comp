#!/usr/bin/env python3
"""Gate A3 (MC-07): contraste WCAG AA da paleta central (src/presentation/qml/Theme.qml).

Lê as cores do Theme.qml (literais e `isDark ? "#escuro" : "#claro"`) e confere, nos dois
temas, cada par texto/fundo usado pela UI: >= 4.5:1 para texto normal. Sai com 1 se algum
par reprovar. Uso: scripts/check_contrast.py [caminho/Theme.qml]
"""
import re
import sys
from pathlib import Path

AA_TEXT = 4.5


def luminance(hex_color):
    h = hex_color.lstrip("#")
    r, g, b = (int(h[i:i + 2], 16) / 255 for i in (0, 2, 4))
    lin = lambda c: c / 12.92 if c <= 0.03928 else ((c + 0.055) / 1.055) ** 2.4
    return 0.2126 * lin(r) + 0.7152 * lin(g) + 0.0722 * lin(b)


def ratio(a, b):
    hi, lo = sorted((luminance(a), luminance(b)), reverse=True)
    return (hi + 0.05) / (lo + 0.05)


def blend(fg, bg, alpha):
    f = [int(fg.lstrip("#")[i:i + 2], 16) for i in (0, 2, 4)]
    b = [int(bg.lstrip("#")[i:i + 2], 16) for i in (0, 2, 4)]
    return "#" + "".join("%02x" % round(alpha * x + (1 - alpha) * y) for x, y in zip(f, b))


def load_palette(path):
    """{nome: (claro, escuro)} a partir das propriedades color do Theme.qml."""
    text = Path(path).read_text(encoding="utf-8")
    raw = {}
    for m in re.finditer(r'property color (\w+):\s*([^\n/]+)', text):
        raw[m.group(1)] = m.group(2).strip()
    pal = {}

    def resolve(expr, dark):
        expr = expr.strip()
        t = re.fullmatch(r'isDark\s*\?\s*(.+?)\s*:\s*(.+)', expr)
        if t:
            return resolve(t.group(1) if dark else t.group(2), dark)
        lit = re.fullmatch(r'"(#[0-9a-fA-F]{6})"', expr)
        if lit:
            return lit.group(1).lower()
        if expr in raw:
            return resolve(raw[expr], dark)
        return None

    for name, expr in raw.items():
        light, dark = resolve(expr, False), resolve(expr, True)
        if light and dark:
            pal[name] = (light, dark)
    return pal


def load_nav_opacity(path):
    m = re.search(r'property real navActiveOpacity:\s*([0-9.]+)', Path(path).read_text(encoding="utf-8"))
    return float(m.group(1)) if m else 0.2


# Pares texto × fundos reais da UI. Fundos de cartão/janela/hover onde texto semântico aparece.
TEXT_ON_SURFACES = ["textColor", "mutedTextColor", "highlightColor", "criticalColor", "successColor",
                    "successStrongColor", "warningColor", "errorColor", "infoColor", "inactiveColor"]
SURFACES = ["windowColor", "cardColor", "sidebarColor", "subtleBg", "insetBg", "hoverColor", "dialogBg"]
FIXED_PAIRS = [  # (texto, fundo) que não dependem do tema
    ("contrastTextColor", "toastSuccessBg"), ("contrastTextColor", "toastErrorBg"),
    ("contrastTextColor", "toastInfoBg"), ("contrastTextColor", "consolePanelBg"),
    ("consoleTextColor", "consoleBg"), ("consoleMutedTextColor", "consolePanelBg"),
]


def translucent_texts(qml_dir):
    """Elementos Text com opacity < 1: o contraste efetivo cai abaixo do medido na paleta."""
    found = []
    for qml in sorted(Path(qml_dir).rglob("*.qml")):
        src = qml.read_text(encoding="utf-8")
        for m in re.finditer(r"\bText\s*\{", src):
            depth, i = 0, m.end() - 1
            while i < len(src):
                depth += {"{": 1, "}": -1}.get(src[i], 0)
                if depth == 0:
                    break
                i += 1
            body = src[m.end():i]
            # só o nível do próprio Text (ignora filhos aninhados)
            own = re.sub(r"\{[^{}]*\}", "", body)
            op = re.search(r"\bopacity:\s*([0-9.]+)", own)
            if op and float(op.group(1)) < 1.0:
                line = src.count("\n", 0, m.start()) + 1
                found.append(f"{qml}:{line}: Text com opacity {op.group(1)} (use mutedTextColor)")
    return found


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "src/presentation/qml/Theme.qml"
    pal = load_palette(path)
    nav_alpha = load_nav_opacity(path)
    failures = []
    for idx, theme in ((0, "claro"), (1, "escuro")):
        for fg in TEXT_ON_SURFACES:
            for bg in SURFACES:
                if fg not in pal or bg not in pal:
                    failures.append(f"[{theme}] cor ausente no Theme.qml: {fg if fg not in pal else bg}")
                    continue
                r = ratio(pal[fg][idx], pal[bg][idx])
                if r < AA_TEXT:
                    failures.append(f"[{theme}] {fg} {pal[fg][idx]} sobre {bg} {pal[bg][idx]}: {r:.2f}:1")
        # Item ativo da navegação: texto highlightColor sobre highlightColor translúcido na sidebar.
        nav_bg = blend(pal["highlightColor"][idx], pal["sidebarColor"][idx], nav_alpha)
        r = ratio(pal["highlightColor"][idx], nav_bg)
        if r < AA_TEXT:
            failures.append(f"[{theme}] navegação ativa: highlightColor sobre destaque {nav_alpha:.0%}: {r:.2f}:1")
        for fg, bg in FIXED_PAIRS:
            if fg not in pal or bg not in pal:
                failures.append(f"cor ausente no Theme.qml: {fg if fg not in pal else bg}")
                continue
            r = ratio(pal[fg][idx], pal[bg][idx])
            if r < AA_TEXT:
                failures.append(f"[{theme}] {fg} sobre {bg}: {r:.2f}:1")
    failures.extend(translucent_texts(Path(path).parent))
    failures = sorted(set(failures))
    for f in failures:
        print("FALHA", f)
    print(f"WCAG AA: {len(failures)} par(es) abaixo de {AA_TEXT}:1" if failures else "WCAG AA: todos os pares >= 4.5:1")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
