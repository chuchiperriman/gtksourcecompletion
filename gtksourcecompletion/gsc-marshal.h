
#ifndef __gtksourcecompletion_marshal_MARSHAL_H__
#define __gtksourcecompletion_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* BOOLEAN:POINTER (gsc-marshal.list:1) */
extern void gtksourcecompletion_marshal_BOOLEAN__POINTER (GClosure     *closure,
                                                          GValue       *return_value,
                                                          guint         n_param_values,
                                                          const GValue *param_values,
                                                          gpointer      invocation_hint,
                                                          gpointer      marshal_data);

/* BOOLEAN:VOID (gsc-marshal.list:2) */
extern void gtksourcecompletion_marshal_BOOLEAN__VOID (GClosure     *closure,
                                                       GValue       *return_value,
                                                       guint         n_param_values,
                                                       const GValue *param_values,
                                                       gpointer      invocation_hint,
                                                       gpointer      marshal_data);

G_END_DECLS

#endif /* __gtksourcecompletion_marshal_MARSHAL_H__ */

