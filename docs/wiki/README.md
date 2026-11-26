# Fonte da wiki

Estas páginas são a fonte versionada da wiki do GitHub (`petrinhu/my_comp.wiki`).
Para publicar (a wiki precisa ter ao menos uma página criada pela interface web antes):

```bash
git clone git@github.com:petrinhu/my_comp.wiki.git /tmp/my_comp.wiki
cp docs/wiki/*.md /tmp/my_comp.wiki/ && rm /tmp/my_comp.wiki/README.md
cd /tmp/my_comp.wiki && git add -A && git commit -m "docs: sincroniza wiki com docs/wiki" && git push
```
