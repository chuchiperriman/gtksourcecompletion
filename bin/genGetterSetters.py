#!/usr/bin/env python
def main():
	header = True
	prefix = 'sc_symbol'
	object_name = 'ScSymbol'
	fields = ['name', 'type_name', 'file', 'language', 'line', 'signature']
	for f in fields:
		if header == True:
			#Getter
			print 'const gchar\t*'+prefix+'_get_'+f+'\t\t('+object_name+' *self);'
			#Setter
			print 'void\t\t '+prefix+'_set_'+f+'\t\t('+object_name+' *self, const gchar *'+f+');'
		else:
			#Getter
			print 'const gchar*'
			print prefix+'_get_'+f+'\t('+object_name+' *self)'
			print '{'
			print '\treturn self->priv->'+f+';'
			print '}'
			#Setter
			print 'void'
			print prefix+'_set_'+f+'\t('+object_name+' *self, gchar *'+f+')'
			print '{'
			print '\tself->priv->'+f+' = g_strdup ('+f+');'
			print '}'
if __name__ == "__main__":
	main()
