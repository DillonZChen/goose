(define (problem ferry-l3-c2)
(:domain ferry)
(:objects l0 l1 l2 
          c0 c1 
)
(:init
(location l0)
(location l1)
(location l2)
(car c0)
(car c1)
(not-eq l0 l1)
(not-eq l1 l0)
(not-eq l0 l2)
(not-eq l2 l0)
(not-eq l1 l2)
(not-eq l2 l1)
(empty-ferry)
(at c0 l1)
(at c1 l1)
(at-ferry l0)
)
(:goal
(and
(at c0 l2)
(at c1 l2)
)
)
)
