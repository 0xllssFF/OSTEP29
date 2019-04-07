Concurrent BST-tree
reference:
H. T. Kung, Philip L. Lehman:
Concurrent Manipulation of Binary Search Trees. ACM Trans. Database Syst. 5(3): 354-382 (1980)

procedure find(n,v)
	f=n;
	if v<f.value then dir=left else dir=right
	s=f.dir;
	if s!=NULL and s.value!=v then 
		return find(s,v)
	else
		lock(f);
		if s!=f.dir then 	//检查 f.dir 是否已经被更改，若被更改，则重新查找
			unlock(f);
			return find(f,v)
		else return (f,dir) 
		
		
procedure search(v)
	(f,dir)=find(root,v);
	s=f.dir;
	if s!=NULL then 
		print "value v is at s"
	else 
		print "value v is not in the tree"
	unlock(f);
	

procedure insert()
	(f,dir)=find(root,v)
	if i.dir != NULL then
    		print"value v is already in the tree"
    		unlock(f)
    	else
    		creat(w)
    		w.left=NULL
    		w.right=NULL
    		w.value=v
 		f.dir=w
    		unlock(f)
