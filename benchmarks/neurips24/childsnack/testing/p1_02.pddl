(define (problem p1_02)
 (:domain childsnack)
 (:objects
   tray1 tray2 - tray
   place1 place2 place3 - place
 )
 (:init (= (hungry kitchen is_gluten_free) 0) (= (hungry place1 is_gluten_free) 3) (= (hungry place1 is_not_gluten_free) 0) (= (hungry place2 is_gluten_free) 7) (= (hungry place2 is_not_gluten_free) 0) (= (hungry place3 is_gluten_free) 5) (= (hungry place3 is_not_gluten_free) 0) (at tray1 kitchen) (= (ontray tray1 is_gluten_free) 0) (= (ontray tray1 is_not_gluten_free) 0) (at tray2 kitchen) (= (ontray tray2 is_gluten_free) 0) (= (ontray tray2 is_not_gluten_free) 0) (= (at_kitchen_bread is_gluten_free) 15) (= (at_kitchen_content is_gluten_free) 15) (= (at_kitchen_sandwich is_gluten_free) 0) (= (at_kitchen_bread is_not_gluten_free) 0) (= (at_kitchen_content is_not_gluten_free) 0) (= (at_kitchen_sandwich is_not_gluten_free) 0) (gluten_free is_gluten_free))
 (:goal (and (= (hungry place1 is_gluten_free) 0) (= (hungry place1 is_not_gluten_free) 0) (= (hungry place2 is_gluten_free) 0) (= (hungry place2 is_not_gluten_free) 0) (= (hungry place3 is_gluten_free) 0) (= (hungry place3 is_not_gluten_free) 0)))
)
