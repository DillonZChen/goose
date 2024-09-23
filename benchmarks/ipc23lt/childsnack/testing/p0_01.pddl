;; children=4, allergic_children=0, trays=1, sandwiches=4, out_folder=testing/easy, instance_id=1, seed=1007

(define (problem childsnack-01)
 (:domain childsnack)
 (:objects 
    child1 child2 child3 child4 - child
    tray1 - tray
    sandw1 sandw2 sandw3 sandw4 - sandwich
    bread1 bread2 bread3 bread4 - bread-portion
    content1 content2 content3 content4 - content-portion
    table1 table2 table3 - place
    )
 (:init 
    (at tray1 kitchen)
    (at_kitchen_bread bread1)
    (at_kitchen_bread bread2)
    (at_kitchen_bread bread3)
    (at_kitchen_bread bread4)
    (at_kitchen_content content1)
    (at_kitchen_content content2)
    (at_kitchen_content content3)
    (at_kitchen_content content4)
    (not_allergic_gluten child2)
    (not_allergic_gluten child4)
    (not_allergic_gluten child3)
    (not_allergic_gluten child1)
    (waiting child2 table3)
    (waiting child4 table1)
    (waiting child3 table3)
    (waiting child1 table1)
    (notexist sandw1)
    (notexist sandw2)
    (notexist sandw3)
    (notexist sandw4))
 (:goal  (and (served child1)
   (served child2)
   (served child3)
   (served child4))))
