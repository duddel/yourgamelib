#!/bin/sh
rm -r _doxyout
rm -r site

doxygen
~/apps/doxybook2 --input _doxyout/xml/ --output ./md_sites --config ./doxybook_cfg.json
mkdocs build
