;; base case
;; graph:
;; l1 - l2
;;  | X |
;; l3 - l4

(define (problem transport-08)
 (:domain transport)
 (:objects
    v1 v2 - vehicle
    p1 - package
    l1 l2 l3 l4 - location
    c0 c1 c2 - size
    )
 (:init
    (capacity v1 c2)
    (capacity v2 c2)
    (capacity-predecessor c0 c1)
    (capacity-predecessor c1 c2)
    (at p1 l2)
    (at v1 l4)
    (at v2 l1)
    (road l1 l2)
    (road l2 l1)
    (road l1 l3)
    (road l3 l1)
    (road l1 l4)
    (road l4 l1)
    (road l3 l2)
    (road l2 l3)
    (road l4 l2)
    (road l2 l4)
    (road l4 l3)
    (road l3 l4)
    )
 (:goal  (and
    (at p1 l3)
 )))
