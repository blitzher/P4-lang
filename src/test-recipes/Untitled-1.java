

public Object visitDoWhileExpression (DoWhile com,Object arg)
{ 
    // Code block to run Block 
    Type CType = (Type)com.C.visit(this,null); 
    
    // Boolean expression
    Type EType  = (Type)com.E.visit(this,null);
    if (! EType.equals(Type.boolT) )
        // report error: type mismatch in expression branches
}
