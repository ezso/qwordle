splint -retvalother -retvalint -branchstate -exportlocal -usedef -compdef -temptrans \
    +charint -predboolint -predboolothers -nullret -nullderef -nullpass -unrecog \
    +matchanyintegral -mustfreefresh -formatcode -mustdefine -initallelements -boolops \
    -compdestroy +posixlib -unqualifiedtrans \
    $(find src -type f ! -iname '*test*')