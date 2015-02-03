#include <cstddef>

#include <Rdefines.h>

#include <xompiler.h>
#include <bytecode.h>

void get_bytecode(const char * args, SEXP * _bce, SEXP * _args) {

    printf("Got '%s'\n", args);

    SEXP bc = PROTECT(NEW_INTEGER(20));

    int * bcc = INTEGER(bc);

    int pc = 0;
    bcc[pc++] = R_bcVersion;
    bcc[pc++] = GETVAR_OP;
    bcc[pc++] = 0;
    bcc[pc++] = LDCONST_OP;
    bcc[pc++] = 1;
    bcc[pc++] = ADD_OP;
    bcc[pc++] = 0; // wtf??
    bcc[pc++] = RETURN_OP;

    SETLENGTH(bc, pc);

    SEXP consts = PROTECT(allocVector(VECSXP, 10));
    SEXP one = PROTECT(allocVector(INTSXP, 1));
    SEXP two = PROTECT(allocVector(INTSXP, 2));
    *INTEGER(one) = 1;
    *INTEGER(two) = 2;

    SEXP a = install("a");

    SEXP arg = PROTECT(CONS(R_MissingArg, R_NilValue));
    SET_TAG(arg, a);

    SET_VECTOR_ELT(consts, 0, a);
    SET_VECTOR_ELT(consts, 1, one);
    SET_VECTOR_ELT(consts, 2, two);
    SETLENGTH(consts, 3);

    SEXP bce = PROTECT(CONS(R_bcEncode(bc), consts));
    SET_TYPEOF(bce, BCODESXP);

    *_bce = bce;
    *_args = arg;

    UNPROTECT(6);
}


extern "C" {
  void xompiler_get_bytecode(SEXP args, SEXP * _bce, SEXP * _args) {
    if (TYPEOF(CAR(args)) == STRSXP) {
      get_bytecode(STRING_VALUE(CAR(args)), _bce, _args);
    } else {
      *_bce = R_NilValue;
      *_args = R_NilValue;
    }
  }
}
