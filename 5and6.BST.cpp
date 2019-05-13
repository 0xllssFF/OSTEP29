Concurrent BST
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
		else return (f,dir)     //注意并没有释放锁，因为find后往往跟随其他操作。若只是单纯的find那么在find返回后将锁释放。
		
		
procedure search(v)		//单纯find
	(f,dir)=find(root,v);
	s=f.dir;
	if s!=NULL then 
		print "value v is at s"
	else 
		print "value v is not in the tree"
	unlock(f);
	

procedure insert()             //find后决定是否插入
	(f,dir)=find(root,v)
	if i.dir != NULL then
    		print"value v is already in the tree"
    		unlock(f)	//find的锁
    	else
    		creat(w)
    		w.left=NULL
    		w.right=NULL
    		w.value=v
 		f.dir=w
    		unlock(f)
