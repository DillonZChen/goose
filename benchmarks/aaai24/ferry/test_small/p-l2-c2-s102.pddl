(define (problem ferry-l2-c2)
(:domain ferry)
(:objects l0 l1 
          c0 c1 
)
(:init
(location l0)
(location l1)
(car c0)
(car c1)
(not-eq l0 l1)
(not-eq l1 l0)
(empty-ferry)
(at c0 l0)
(at c1 l0)
(at-ferry l1)
)
(:goal
(and
(at c0 l0)
(at c1 l0)
)
)
)
