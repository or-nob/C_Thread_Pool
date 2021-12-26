#!/bin/bash
make -nw>tmp.txt
file_name='tmp.txt'
cnt=0
command=()
while read line;do
    if ! [[ $line =~ mkdir* ]] && ! [[ $line =~ make:* ]];then
        command+=( "$line" )
    fi
    if [[ $line =~ mkdir* ]];then
        $line
    fi
done<$file_name

for tok in "${command[@]::${#command[@]}-1}";do
    tok=$(echo "${tok/c++/clang++}")
    tok=$(echo "${tok/cc/clang}")
    tok_arr=($(echo $tok|tr " " "\n"))
    last_tok=${tok_arr[${#tok_arr[@]} - 1]}
    last_tok=$(echo "${last_tok/.cpp.o/.o.json}")
    last_tok=$(echo "${last_tok/.c.o/.o.json}")
    ${tok} -MJ ${last_tok}
done

sed -e '1s/^/[\'$'\n''/' -e '$s/,$/\'$'\n'']/' */*/*.o.json > compile_commands.json
rm */*/*.o.json
rm tmp.txt
