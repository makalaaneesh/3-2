INFINITY = float("inf")

def cut_rod_recursive(p, n):

	"""
	p - price array
	n - length of the rod
	"""
	if n == 0:
		return 0
	q = 0
	for i in range(1, n+1):
		# print "Calling RECURSIVE ", n-i
		a = p[i] + cut_rod_recursive(p, n-i)
		q = max(q, a) 
		# print "returned RECURSIVE ", n-i
		print n,"->",a, " (by cutting ", i, ",",n-i,"). MAX is ", q
	return q





if __name__ == "__main__":
	p = [0, 1,1,8,9,10,17,17,20,24,30]
	print cut_rod_recursive(p, 6)