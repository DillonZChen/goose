(define (problem DLOG-2-2-2)
	(:domain driverlog)
	(:objects
	driver1 - driver
	driver2 - driver
	truck1 - truck
	truck2 - truck
	package1 - obj
	package2 - obj
	s0 - location
	s1 - location
	s2 - location
	p0-1 - location
	p0-2 - location
	p1-0 - location
	p1-2 - location
	)
	(:init
	(at-driver driver1 s2)
	(= (walked driver1) 0)
	(at-driver driver2 s0)
	(= (walked driver2) 0)
	(at-truck truck1 s2)
	(empty truck1)
	(= (driven truck1) 0)
	(at-truck truck2 s1)
	(empty truck2)
	(= (driven truck2) 0)
	(at-pkg package1 s2)
	(at-pkg package2 s0)
	(path s0 p0-1)
	(path p0-1 s0)
	(path s1 p0-1)
	(path p0-1 s1)
	(= (time-to-walk s0 p0-1) 2)
	(= (time-to-walk p0-1 s0) 2)
	(= (time-to-walk s1 p0-1) 68)
	(= (time-to-walk p0-1 s1) 68)
	(path s0 p0-2)
	(path p0-2 s0)
	(path s2 p0-2)
	(path p0-2 s2)
	(= (time-to-walk s0 p0-2) 68)
	(= (time-to-walk p0-2 s0) 68)
	(= (time-to-walk s2 p0-2) 37)
	(= (time-to-walk p0-2 s2) 37)
	(path s1 p1-2)
	(path p1-2 s1)
	(path s2 p1-2)
	(path p1-2 s2)
	(= (time-to-walk s1 p1-2) 44)
	(= (time-to-walk p1-2 s1) 44)
	(= (time-to-walk s2 p1-2) 17)
	(= (time-to-walk p1-2 s2) 17)
	(link s1 s0)
	(link s0 s1)
	(= (time-to-drive s1 s0) 27)
	(= (time-to-drive s0 s1) 27)
	(link s1 s2)
	(link s2 s1)
	(= (time-to-drive s1 s2) 3)
	(= (time-to-drive s2 s1) 3)
	(link s2 s0)
	(link s0 s2)
	(= (time-to-drive s2 s0) 21)
	(= (time-to-drive s0 s2) 21)
)
	(:goal (and
	(at-driver driver2 s0)
	(at-truck truck2 s2)
	(at-pkg package2 s2)
	))

(:metric minimize (walked driver1) (walked driver2) (driven truck1) (driven truck2))

)
