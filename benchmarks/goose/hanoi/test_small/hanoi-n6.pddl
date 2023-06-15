(define (problem hanoi-6)
  (:domain hanoi-domain)
  (:objects peg1 peg2 peg3 d1 d2 d3 d4 d5 d6 )
  (:init 
    (smaller d1 peg1)(smaller d1 peg2)(smaller d1 peg3)
    (smaller d2 peg1)(smaller d2 peg2)(smaller d2 peg3)
    (smaller d3 peg1)(smaller d3 peg2)(smaller d3 peg3)
    (smaller d4 peg1)(smaller d4 peg2)(smaller d4 peg3)
    (smaller d5 peg1)(smaller d5 peg2)(smaller d5 peg3)
    (smaller d6 peg1)(smaller d6 peg2)(smaller d6 peg3)

    (smaller d1 d2)(smaller d1 d3)(smaller d1 d4)(smaller d1 d5)(smaller d1 d6)
    (smaller d2 d3)(smaller d2 d4)(smaller d2 d5)(smaller d2 d6)
    (smaller d3 d4)(smaller d3 d5)(smaller d3 d6)
    (smaller d4 d5)(smaller d4 d6)
    (smaller d5 d6)
    
    (clear peg1)(clear peg2)(clear d1)
    
    (on d1 d2)(on d2 d3)(on d3 d4)(on d4 d5)(on d5 d6)(on d6 peg3)
  )
  (:goal 
    (and (on d1 d2)(on d2 d3)(on d3 d4)(on d4 d5)(on d5 d6)(on d6 peg1) )
  )
)
