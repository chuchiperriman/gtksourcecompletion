#!/bin/sh
# Crea un provider a partir del provider test
com="s/gtksourcecompletion/gtksourceview/g;s/gsc-/gtksourcecompletion/g"
com="$com;s/GSC/GTK_SOURCE_COMPLETION/g"
com="$com;s/Gsc/GtkSourceCompletion/g"
com="$com;s/gsc/gtk_source_completion/g"

com="sed -i $com"

for fl in *.[ch]; do
	echo $fl
	$com $fl
done

#find *.[ch] -type f -exec $com {} \;
