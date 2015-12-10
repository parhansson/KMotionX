import time, sys
try:
    import kmotion
    from kmotion_extras import *
except:
    print "Failed to import kmotion module"
    print "Did you forget to..."
    print "export PYTHONPATH=<KMmotionX install path>/bin"
    print "export KMOTION_BIN=<KMmotionX install path>/bin"
    raise



k = KMotionX(0, with_console=True)


usage = """
Usage: python testcomp.py [-g] [-c] [-n] [-D...] [-I...] <code> [<compiler>]
   -g  Debug info
   -tn Thread n (default 1)
   -c  Compile only (else compile and run)
   -n  Don't wait for thread to finish execution
   -D...  Passed to compiler options (defines)
   -I...  Passed to compiler options (include path)
   -l...  Passed to compiler options without the -l (extra .out files for linking)
   <code> is file to compile or index of code string in testcomp.py
   <compiler> is TCC compiler to use (default tcc67).
"""
if len(sys.argv) < 2:
    print usage
    sys.exit(1)
comp_only = False
nowait = False
codestring = None
compilerbin = None
opts = ""
thread = 1
for a in sys.argv[1:]:
    if a == "-c":
        comp_only = True
    elif a == "-g":
        opts += "-g "
    elif a == "-n":
        nowait = True
    elif a.startswith("-D"):
        k.tmgr.add_define(a[2:])
    elif a.startswith("-I"):
        k.tmgr.add_incldir(a[2:])
    elif a.startswith("-l"):
        opts += a[2:] + " "
    elif a.startswith("-t"):
        thread = int(a[2:])
    elif codestring is None:
        codestring = a
    elif compilerbin is None:
        compilerbin = a
    else:
        print "Arg", a, "not recognized."
        print usage
        sys.exit(1)

if codestring is None:
    print "Missing code to run."
    print usage
    sys.exit(1)
if compilerbin is None:
    compilerbin = "tcc67"        

# Set the compiler: "tcc67", or "cl6x" (only if TI compiler tools installed).
# Can also specify "tcc67wine" but you will need to set up a script of that
# name in the bin directory which invokes TCC67 under wine.  Such a script
# might look like
#   #!/bin/bash
#   wine ~/.wine/drive_c/KMotion432/KMotion/Release/TCC67.exe $*
k.tmgr.set_cc(compilerbin)
k.tmgr.set_opts(opts)

def poll(k):
    k.Poll()
    while True:
        m = k.get_message()
        if not m: break
        print m
        if m.is_fatal():
            # Kflop comms error
            print "Kflop disconnected!"

def idle_func(kthread):
    global k
    poll(k)



code1 = r"""
#include "KMotionDef.h"

int fac(int n)
{
    return n ? n*fac(n-1) : 1;
}
void main()
{
    int i = %d;
    
    printf("This is a very roundabout way of computing that %%d! = %%d\n", i, fac(i));
    printf("Python string!\n");

}
""" % (8,)

code2 = r"""
#include "KMotionDef.h"

#define fabs fast_fabs


#define M 20
#define N 20

static const double epsilon   = 1.0e-8;
int equal(double a, double b) { return fabs(a-b) < epsilon; }
void assert(int b, char *s) {if (!b) printf("ASSERT FAIL %s\n",s);}

typedef struct {
  int m, n; // m=rows, n=columns, mat[m x n]
  double mat[M][N];
} Tableau;

void nl(int k){ int j; for(j=0;j<k;j++) printf("-"); printf("\n"); }

void print_tableau(Tableau *tab, const char* mes) {
  static int counter=0;
  int i, j;
  printf("\n%d. Tableau %s:\n", ++counter, mes);
  nl(70);

  printf("%-6s%5s", "col:", "b[i]");
  for(j=1;j<tab->n; j++) { printf("    x%d,", j); } printf("\n");

  for(i=0;i<tab->m; i++) {
    if (i==0) printf("max:"); else
    printf("b%d: ", i);
    for(j=0;j<tab->n; j++) {
      if (equal((int)tab->mat[i][j], tab->mat[i][j]))
        printf(" %6d", (int)tab->mat[i][j]);
      else
        printf(" %6.2lf", tab->mat[i][j]);
      }
    printf("\n");
  }
  nl(70);
}


void pivot_on(Tableau *tab, int row, int col) {
  int i, j;
  double pivot;

  pivot = tab->mat[row][col];
  assert(pivot>0,"pivot>0");
  for(j=0;j<tab->n;j++)
    tab->mat[row][j] /= pivot;
  assert( equal(tab->mat[row][col], 1. ),"equal(tab->mat[row][col], 1. )");

  for(i=0; i<tab->m; i++) { // foreach remaining row i do
    double multiplier = tab->mat[i][col];
    if(i==row) continue;
    for(j=0; j<tab->n; j++) { // r[i] = r[i] - z * r[row];
      tab->mat[i][j] -= multiplier * tab->mat[row][j];
    }
  }
}

// Find pivot_col = most negative column in mat[0][1..n]
int find_pivot_column(Tableau *tab) {
  int j, pivot_col = 1;
  double lowest = tab->mat[0][pivot_col];
  for(j=1; j<tab->n; j++) {
    if (tab->mat[0][j] < lowest) {
      lowest = tab->mat[0][j];
      pivot_col = j;
    }
  }
  printf("Most negative column in row[0] is col %d = %g.\n", pivot_col, lowest);
  if( lowest >= 0 ) {
    return -1; // All positive columns in row[0], this is optimal.
  }
  return pivot_col;
}

// Find the pivot_row, with smallest positive ratio = col[0] / col[pivot]
int find_pivot_row(Tableau *tab, int pivot_col) {
  int i, pivot_row = 0;
  double min_ratio = -1;
  printf("Ratios A[row_i,0]/A[row_i,%d] = [",pivot_col);
  for(i=1;i<tab->m;i++){
    double ratio = tab->mat[i][0] / tab->mat[i][pivot_col];
    printf("%3.2lf, ", ratio);
    if ( (ratio > 0  && ratio < min_ratio ) || min_ratio < 0 ) {
      min_ratio = ratio;
      pivot_row = i;
    }
  }
  printf("].\n");
  if (min_ratio == -1)
    return -1; // Unbounded.
  printf("Found pivot A[%d,%d], min positive ratio=%g in row=%d.\n",
      pivot_row, pivot_col, min_ratio, pivot_row);
  return pivot_row;
}

void add_slack_variables(Tableau *tab) {
  int i, j;
  for(i=1; i<tab->m; i++) {
    for(j=1; j<tab->m; j++)
      tab->mat[i][j + tab->n -1] = (i==j);
  }
  tab->n += tab->m -1;
}

void check_b_positive(Tableau *tab) {
  int i;
  for(i=1; i<tab->m; i++)
	assert(tab->mat[i][0] >= 0,"tab->mat[i][0] >= 0");
}

// Given a column of identity matrix, find the row containing 1.
// return -1, if the column as not from an identity matrix.
int find_basis_variable(Tableau *tab, int col) {
  int i, xi=-1;
  for(i=1; i < tab->m; i++) {
    if (equal( tab->mat[i][col],1) ) {
      if (xi == -1)
        xi=i;   // found first '1', save this row number.
      else
        return -1; // found second '1', not an identity matrix.

    } else if (!equal( tab->mat[i][col],0) ) {
      return -1; // not an identity matrix column.
    }
  }
  return xi;
}

void print_optimal_vector(Tableau *tab, char *message) {
  int j, xi;
  printf("%s at ", message);
  for(j=1;j<tab->n;j++) { // for each column.
    xi = find_basis_variable(tab, j);
    if (xi != -1)
      printf("x%d=%3.2lf, ", j, tab->mat[xi][0] );
    else
      printf("x%d=0, ", j);
  }
  printf("\n");
} 

void simplex(Tableau *tab) {
  int loop=0;
  add_slack_variables(tab);
  check_b_positive(tab);
  print_tableau(tab,"Padded with slack variables");
  while( ++loop ) {
    int pivot_col, pivot_row;

    pivot_col = find_pivot_column(tab);
    if( pivot_col < 0 ) {
      printf("Found optimal value=A[0,0]=%3.2lf (no negatives in row 0).\n",
        tab->mat[0][0]);
      print_optimal_vector(tab, "Optimal vector ");
      break;
    }
    printf("Entering variable x%d to be made basic, so pivot_col=%d.\n",
      pivot_col, pivot_col);

    pivot_row = find_pivot_row(tab, pivot_col);
    if (pivot_row < 0) {
      printf("unbounded (no pivot_row).\n");
      break;
    }
    printf("Leaving variable x%d, so pivot_row=%d\n", pivot_row, pivot_row);

    pivot_on(tab, pivot_row, pivot_col);
    print_tableau(tab,"After pivoting");
    print_optimal_vector(tab, "Basic feasible solution");

    if(loop > 20) {
      printf("Too many iterations > %d.\n", loop);
      break;
    }
  }
}

Tableau tab  = { 4, 5, {                     // Size of tableau [4 rows x 5 columns ]
    {  0.0 , -0.5 , -3.0 ,-1.0 , -4.0,   },  // Max: z = 0.5*x + 3*y + z + 4*w,
    { 40.0 ,  1.0 ,  1.0 , 1.0 ,  1.0,   },  //    x + y + z + w <= 40 .. b1
    { 10.0 , -2.0 , -1.0 , 1.0 ,  1.0,   },  //  -2x - y + z + w <= 10 .. b2
    { 10.0 ,  0.0 ,  1.0 , 0.0 , -1.0,   },  //        y     - w <= 10 .. b3
  }
};

int main(int argc, char *argv[]){
  print_tableau(&tab,"Initial");
  simplex(&tab);
  printf("Correct Answer 145 x1=0, x2=15.00, x3=0, x4=25.00, x5=0, x6=0, x7=20.00, \n");
  return 0;
} 
"""

code3 = r"""
#include "KMotionDef.h"


typedef struct
{
    float    spd;
    double  postmove;
    int        limsw;
} home_t;

home_t h[1];

main()
{
    int i=0;
    double p = -2000.0;
    h[i].spd = 456;
    h[i].postmove = -2000;
    h[i].limsw = 123;
    if (h[i].postmove == 0.0)
        printf("(zero) - bad\n");
    else
        printf("(non-zero) - good\n");
    printf("Values are %g, %g, %d\n", h[i].spd, h[i].postmove, h[i].limsw);
    printf("postmove = %g\n", h[i].postmove);
    printf("constant = %g\n", -2000.0);
    printf("double = %g\n", p);
    p = h[i].postmove;
    printf("double = %g should be -2000\n", p);
    p = h[i].postmove + 10000.0;
    printf("double = %g  should be 8000\n", p);
    Delay_sec(0.1);
}


"""
cs = "code"+codestring
    
if cs in globals():
    cs = globals()[cs]
    tmpname = "/tmp/testkmotion.c"
    with open(tmpname, "w") as f:
        f.write(cs)
    codestring = tmpname
    
if comp_only:
    print "Compile file..."
    k.tmgr.run(codestring, thread, compile_opts=ThreadManager.ONLY)
else:
    print "Compile and run file..."
    k.run_c(codestring, thread)
    if not nowait:
        k.wait_c(thread, idle_func)
        
poll(k)

print "done!"

        


