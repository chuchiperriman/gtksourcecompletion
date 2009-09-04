#!/bin/sh
# Crea un provider a partir del provider test

orig_ns=GTK
orig_name=SOURCE_COMPLETION
orig_type=GtkSourceCompletion

new_ns=GSC
new_name=COMPLETION
new_type=GscCompletion


orig_mays=${orig_ns}_${orig_name}
orig_mays=$(echo $orig_mays | tr '[:lower:]' '[:upper:]')
orig_min=$(echo $orig_mays | tr '[:upper:]' '[:lower:]')
orig_file=$(echo $orig_min | tr '_' '-')
orig_is=${orig_ns}_IS_${orig_name}
orig_is=$(echo $orig_is | tr '[:lower:]' '[:upper:]')
orig_ptype=${orig_ns}_TYPE_${orig_name}
orig_ptype=$(echo $orig_ptype | tr '[:lower:]' '[:upper:]')

new_mays=${new_ns}_${new_name}
new_mays=$(echo $new_mays | tr '[:lower:]' '[:upper:]')
new_min=$(echo $new_mays | tr '[:upper:]' '[:lower:]')
new_file=$(echo $new_min | tr '_' '-')
new_is=${new_ns}_IS_${new_name}
new_is=$(echo $new_is | tr '[:lower:]' '[:upper:]')
new_ptype=${new_ns}_TYPE_${new_name}
new_ptype=$(echo $new_ptype | tr '[:lower:]' '[:upper:]')

echo "----------"
echo -e "$orig_type \t $new_type"
echo -e "$orig_mays \t $new_mays"
echo -e "$orig_min \t $new_min"
echo -e "$orig_file \t $new_file"
echo -e "$orig_is \t $new_is"
echo -e "$orig_ptype \t $new_ptype"
echo "----------"


com="s/$orig_mays/$new_mays/g;s/$orig_ns/$new_ns/g;s/$orig_type/$new_type/g"
com="$com;s/$orig_min/$new_min/g;s/${orig_file}/${new_file}/g"
com="$com;s/${orig_is}/${new_is}/g;s/${orig_ptype}/${new_ptype}/g"
com="$com;s/__${orig_mays}_H__/__${new_mays}_H__/g;s/__${orig_name}_H__/__${new_name}_H__/g"

com="sed -i $com"

#for fl in *.[ch]; do
#for fl in ${new_file}.[ch]; do
for fl in *.[ch]; do
	echo $fl
	$com $fl
done

#find *.[ch] -type f -exec $com {} \;
