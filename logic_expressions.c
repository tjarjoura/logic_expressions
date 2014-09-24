#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_IDENT 1024
#define MAX_LINE 4096

char identifiers[MAX_IDENT];
int  truth_values[MAX_IDENT];

int no_identifiers = 0;

int isop(char c) {
   switch(c) {
      case '|':
      case '&':
      case '>':
      case '=':
      case '-':
         return 1;
      default:
         return 0;
   }
}

void sort_identifiers()
{
   int i, j, key;

   for (j = 1; j < no_identifiers; j++) {
      key = identifiers[j];
      i = j-1;

      while ((i >= 0) && (identifiers[i] > key)) {
         identifiers[i+1] = identifiers[i];
         i--;
      }

      identifiers[i+1] = key;
   }
}

void get_identifiers(char* buf, int len)
{
   int i;

   for (i = 0; i < len; i++)
      if (isalpha(buf[i]) && (strchr(identifiers, buf[i]) == NULL)) {
         identifiers[no_identifiers] = buf[i];
         truth_values[no_identifiers++] = 1;
      }
}

int shunting_yard(char* expr, int len)
{
   char rpn_expr[MAX_LINE];
   char opstack[MAX_LINE];

   int i;
   int j = 0; // rpn_expr index
   int k = 0; // opstack size

   for (i = 0; i < len; i++) {
      if (isalpha(expr[i])) 
         rpn_expr[j++] = expr[i];
      
      else if (isop(expr[i])) {
         if (expr[i] == '-')
            opstack[k++] = expr[i];
         
         else {
            while (isop(opstack[k-1])) 
               rpn_expr[j++] = opstack[--k];
            opstack[k++] = expr[i];
         }
      }

      else if (expr[i] == '(') 
         opstack[k++] = '(';

      else if (expr[i] == ')') {
         while((k > 0) && (opstack[k-1] != '(')) 
            rpn_expr[j++] = opstack[--k];
         
         if (opstack[k-1] == '(')
            k--;
         else if (k <= 0) {
            printf("Error: Mismatched parenthesis, at %c\n", expr[i]);
            return -1;
         }
      }

      else if (isspace(expr[i]))
         continue;

      else {
         printf("Error: illegal character \'%c\' in expression.\n", expr[i]);
         return -1;
      }
   }

   while (k > 0) {
      if ((opstack[k-1] == '(') || (opstack[k-1] == ')')) {
         printf("Error: Mismatched parenthesis.\n");
         return -1;
      }
      rpn_expr[j++] = opstack[--k];
   }

   rpn_expr[j] = '\0';

   printf("rpn_expr: %s\n", rpn_expr);

   strncpy(expr, rpn_expr, MAX_LINE); /* write over infix with reverse polish notation expression */

   printf("expr: %s\n", expr);
}

int evaluate(char* expr, int len)
{
   int eval_stack[MAX_LINE];
   int top = 0;
   int i;
   int j; //index finder
   int x, y;

   for (i = 0; i < len; i++) {
      if (isalpha(expr[i])) {
         for (j = 0; identifiers[j] != expr[i]; j++) ; //increment j to proper index
         eval_stack[top++] = truth_values[j]; // push value onto stack
      }

      else if (isop(expr[i])) {
            switch (expr[i]) {
               case '-':
                  eval_stack[top-1] = !eval_stack[top-1];
                  break;
               case '&':
                  x = eval_stack[--top];
                  y = eval_stack[--top];
                  eval_stack[top++] = (x && y);
                  break;
               case '|':
                  x = eval_stack[--top];
                  y = eval_stack[--top];
                  eval_stack[top++] = (x || y);
                  break;
               case '>':
                  x = eval_stack[--top];
                  y = eval_stack[--top];
                  eval_stack[top++] = (!y || (x && y));
                  break;
               case '=':
                  x = eval_stack[--top];
                  y = eval_stack[--top];
                  eval_stack[top++] = ((x && y) || (!x && !y));
                  break;
            }
      }
   }

   return eval_stack[top-1];
}

void print_truth_table(char* expr)
{
   int runs = pow(2, no_identifiers);
   int i, j;
   int testval;

   for (i = 0; i < no_identifiers; i++)
      printf("%c ", identifiers[i]);
   printf("R\n");

   for (i = 0; i < runs; i++) {
      testval = 1;

      for (j = (no_identifiers-1); j >= 0; j--) { /*set appropriate truth values */
         if (((i % testval) == 0) && (i != 0))
            truth_values[j] = !(truth_values[j]);
         testval *= 2; /* truth value changes twice as slow as you go left in truth table */
      }

      for (j = 0; j < no_identifiers; j++) {
         if (truth_values[j] == 1)
            printf("T ");
         else
            printf("F ");
      }

      if (evaluate(expr, strlen(expr)) == 1)
         printf("T\n");
      else
         printf("F\n");
   }
}

int main(int argc, char** argv)
{
   char expr_buffer[MAX_LINE];
   
   fgets(expr_buffer, MAX_LINE, stdin);
   *(strchr(expr_buffer, '\n')) = '\0';

   get_identifiers(expr_buffer, strlen(expr_buffer));
   sort_identifiers();

   if (shunting_yard(expr_buffer, strlen(expr_buffer)) != -1)
      print_truth_table(expr_buffer);

   return 0;
}
