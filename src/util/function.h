#include "program_context.h"

gboolean
add(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
    context->reg.Y = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));

    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->reg.X + context->reg.Y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}

gboolean
and(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
    context->reg.Y = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));

    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->reg.X & context->reg.Y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}

gboolean
drop(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 1)
    g_queue_pop_head (program_context_get_stack_data(context));
  else
    g_print ("ERROR: pila vacia\n");

  return FALSE;
}

gboolean
dupl(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  gpointer ptr;

  if(g_queue_get_length (program_context_get_stack_data(context)) >= 1)
  {
    ptr = g_queue_peek_head (program_context_get_stack_data(context));
    context->reg.X = GPOINTER_TO_INT (ptr);
    context->reg.Y = 0;
    g_queue_push_head (program_context_get_stack_data(context), ptr);
  }
  else
  {
    g_print ("ERROR: pila vacia\n");
  }

  return FALSE;
}

gboolean
neg(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 1)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
    context->reg.Y = -1;
    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->reg.X * context->reg.Y));
  }
  else
  {
    g_print ("ERROR: pila vacia\n");
  }

  return FALSE;
}

gboolean
dec(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 1)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
    context->reg.Y = -1;
    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (--context->reg.X));
  }
  else
  {
    g_print ("ERROR: pila vacia\n");
  }

  return FALSE;
}

gboolean
inc(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 1)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
    context->reg.Y = 1;
    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (++context->reg.X));
  }
  else
  {
    g_print ("ERROR: pila vacia\n");
  }

  return FALSE;
}

gboolean
eq(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_peek_nth (program_context_get_stack_data(context), 0));
    context->reg.Y = GPOINTER_TO_INT (g_queue_peek_nth (program_context_get_stack_data(context), 1));
    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->reg.X == context->reg.Y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}

gboolean
ne(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_peek_nth (program_context_get_stack_data(context), 0));
    context->reg.Y = GPOINTER_TO_INT (g_queue_peek_nth (program_context_get_stack_data(context), 1));
    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->reg.X != context->reg.Y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}

gboolean
ge(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_peek_nth (program_context_get_stack_data(context), 0));
    context->reg.Y = GPOINTER_TO_INT (g_queue_peek_nth (program_context_get_stack_data(context), 1));
    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->reg.X >= context->reg.Y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}

gboolean
le(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_peek_nth (program_context_get_stack_data(context), 0));
    context->reg.Y = GPOINTER_TO_INT (g_queue_peek_nth (program_context_get_stack_data(context), 1));
    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->reg.X <= context->reg.Y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}

gboolean
gt(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_peek_nth (program_context_get_stack_data(context), 0));
    context->reg.Y = GPOINTER_TO_INT (g_queue_peek_nth (program_context_get_stack_data(context), 1));
    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->reg.X > context->reg.Y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}

gboolean
lt(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_peek_nth (program_context_get_stack_data(context), 0));
    context->reg.Y = GPOINTER_TO_INT (g_queue_peek_nth (program_context_get_stack_data(context), 1));
    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->reg.X < context->reg.Y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}

gboolean
jz(ProgramContext *context, gpointer data)
{
  tag_t *tag = data;

  if(g_queue_get_length (program_context_get_stack_data(context)) >= 1)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
    context->reg.Y = 0;

    if(context->reg.X == context->reg.Y) {
      context->label_execute = tag->node;
      context->reg.PC = tag->IR;
      return TRUE;
    }
  }
  else
  {
    g_print ("ERROR: pila vacia\n");
  }

  return FALSE;
}

gboolean
jnz(ProgramContext *context, gpointer data)
{
  tag_t *tag = data;

  if(g_queue_get_length (program_context_get_stack_data(context)) >= 1)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
    context->reg.Y = 0;

    if(context->reg.X != context->reg.Y) {
      context->label_execute = tag->node;
      context->reg.PC = tag->IR;
      return TRUE;
    }
  }
  else
  {
    g_print ("ERROR: pila vacia\n");
  }

  return FALSE;
}

gboolean
divi(G_GNUC_UNUSED ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
    context->reg.Y = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));

    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->reg.X / context->reg.Y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}

gboolean
mul(G_GNUC_UNUSED ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
    context->reg.Y = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));

    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->reg.X * context->reg.Y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}

gboolean
not(G_GNUC_UNUSED ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 1)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));

    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (~context->reg.X));
  }

  return FALSE;
}

gboolean
or(G_GNUC_UNUSED ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    context->reg.X = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
    context->reg.Y = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));

    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->reg.X | context->reg.Y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}

gboolean
pop(ProgramContext *context, gpointer data)
{
  context->reg.X = context->reg.Y = 0;
  *((gint64 *) data) = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));

  return FALSE;
}


gboolean
push(ProgramContext *context, gpointer data)
{
  context->reg.X = context->reg.Y = 0;
  g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (*((gint64 *) data)));

  return FALSE;
}

gboolean
popout(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  gint index = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
  
  if(index < 2)
  {
    context->puertos[index] =  GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
  }
  else
  {
    g_print ("ERROR: numero de puerto no existe\n");
  }

  return FALSE;
}

gboolean
popin(ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  gint index = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
  
  if(index < 2)
  {
    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (context->puertos[index]));
  }
  else
  {
    g_print ("ERROR: numero de puerto no existe\n");
  }

  return FALSE;
}

gboolean
sub(G_GNUC_UNUSED ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  gint64 x, y;

  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    x = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
    y = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));

    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (x - y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}

gboolean
xor(G_GNUC_UNUSED ProgramContext *context, G_GNUC_UNUSED gpointer data)
{
  gint64 x, y;

  if(g_queue_get_length (program_context_get_stack_data(context)) >= 2)
  {
    x = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));
    y = GPOINTER_TO_INT (g_queue_pop_head (program_context_get_stack_data(context)));

    g_queue_push_head (program_context_get_stack_data(context), GINT_TO_POINTER (x ^ y));
  }
  else
  {
    g_print ("ERROR: tamaño de pila es menor a 2\n");
  }

  return FALSE;
}