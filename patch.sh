# for the GUI you might need
# noto-fonts-emoji, gtk4
echo "Initialize support for headerfiles in VSCode"
echo -e "CompileFlags:\n  Add:\n    - \"--include-directory=$(pwd)/include\"" > .clangd