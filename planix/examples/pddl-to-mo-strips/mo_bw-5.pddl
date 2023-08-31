(define (problem block-5-1)
  (:domain blocksworld)
  
  (:objects B1 B2 B3 B4 B5)
  
  (:init (on-table B1)
	 (on B2 B1)
	 (on-table B3)
	 (on B4 B3)
	 (on-table B5)
	 (clear B2)
	 (clear B4)
	 (clear B5)
	 (= (moves-to-table) 0)
	 (= (moves-to-block) 0)
	 (= (total-moves) 0)
	 )
  
  (:goal (and (on B3 B2) (on B4 B3) (on B5 B1)))

  (:metric minimize (moves-to-table)
             (moves-to-block)
          (total-moves))
)

