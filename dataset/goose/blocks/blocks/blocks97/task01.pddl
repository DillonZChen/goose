(define (problem BW-97-1-1)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47 b48 b49 b50 b51 b52 b53 b54 b55 b56 b57 b58 b59 b60 b61 b62 b63 b64 b65 b66 b67 b68 b69 b70 b71 b72 b73 b74 b75 b76 b77 b78 b79 b80 b81 b82 b83 b84 b85 b86 b87 b88 b89 b90 b91 b92 b93 b94 b95 b96 b97 - block)
    (:init
        (handempty)
        (on b1 b93)
        (on b2 b5)
        (on b3 b90)
        (on b4 b48)
        (on b5 b37)
        (on b6 b14)
        (on b7 b29)
        (on-table b8)
        (on b9 b74)
        (on b10 b11)
        (on b11 b77)
        (on-table b12)
        (on b13 b19)
        (on b14 b20)
        (on b15 b68)
        (on b16 b21)
        (on b17 b94)
        (on b18 b73)
        (on-table b19)
        (on b20 b13)
        (on b21 b66)
        (on b22 b83)
        (on b23 b46)
        (on b24 b39)
        (on b25 b30)
        (on b26 b80)
        (on b27 b15)
        (on b28 b38)
        (on b29 b43)
        (on b30 b59)
        (on b31 b23)
        (on b32 b1)
        (on b33 b35)
        (on b34 b53)
        (on b35 b12)
        (on b36 b95)
        (on-table b37)
        (on b38 b65)
        (on b39 b16)
        (on b40 b56)
        (on-table b41)
        (on b42 b60)
        (on b43 b63)
        (on b44 b32)
        (on b45 b57)
        (on b46 b91)
        (on b47 b27)
        (on b48 b61)
        (on b49 b41)
        (on b50 b17)
        (on b51 b78)
        (on b52 b40)
        (on b53 b6)
        (on b54 b50)
        (on b55 b28)
        (on b56 b4)
        (on b57 b25)
        (on b58 b18)
        (on b59 b33)
        (on b60 b64)
        (on b61 b92)
        (on b62 b67)
        (on b63 b54)
        (on b64 b51)
        (on b65 b10)
        (on b66 b49)
        (on b67 b89)
        (on b68 b88)
        (on b69 b26)
        (on b70 b79)
        (on b71 b96)
        (on b72 b82)
        (on-table b73)
        (on b74 b22)
        (on b75 b9)
        (on b76 b45)
        (on b77 b84)
        (on b78 b47)
        (on b79 b52)
        (on b80 b42)
        (on b81 b3)
        (on b82 b76)
        (on b83 b31)
        (on b84 b58)
        (on b85 b81)
        (on b86 b36)
        (on b87 b62)
        (on b88 b87)
        (on b89 b86)
        (on-table b90)
        (on b91 b72)
        (on b92 b55)
        (on b93 b34)
        (on b94 b70)
        (on b95 b71)
        (on b96 b85)
        (on b97 b44)
        (clear b2)
        (clear b7)
        (clear b8)
        (clear b24)
        (clear b69)
        (clear b75)
        (clear b97)
    )
    (:goal
        (and
            (on b1 b63)
            (on-table b2)
            (on b3 b15)
            (on b4 b90)
            (on b5 b69)
            (on b6 b5)
            (on b7 b4)
            (on b8 b47)
            (on b9 b18)
            (on-table b10)
            (on b11 b54)
            (on b12 b13)
            (on b13 b50)
            (on b14 b64)
            (on b15 b16)
            (on b16 b8)
            (on b17 b33)
            (on b18 b86)
            (on b19 b37)
            (on b20 b96)
            (on b21 b85)
            (on b22 b14)
            (on b23 b93)
            (on b24 b32)
            (on b25 b82)
            (on b26 b11)
            (on b27 b55)
            (on b28 b56)
            (on-table b29)
            (on b30 b38)
            (on-table b31)
            (on b32 b9)
            (on b33 b72)
            (on b34 b30)
            (on b35 b3)
            (on-table b36)
            (on b37 b88)
            (on b38 b23)
            (on b39 b17)
            (on b40 b52)
            (on b41 b22)
            (on b42 b79)
            (on-table b43)
            (on b44 b62)
            (on b45 b68)
            (on b46 b49)
            (on-table b47)
            (on b48 b36)
            (on b49 b81)
            (on b50 b73)
            (on b51 b44)
            (on b52 b12)
            (on b53 b34)
            (on b54 b94)
            (on b55 b70)
            (on b56 b67)
            (on b57 b61)
            (on-table b58)
            (on b59 b39)
            (on b60 b51)
            (on b61 b95)
            (on b62 b57)
            (on-table b63)
            (on b64 b10)
            (on b65 b59)
            (on b66 b87)
            (on b67 b97)
            (on b68 b58)
            (on-table b69)
            (on b70 b76)
            (on b71 b40)
            (on b72 b25)
            (on b73 b83)
            (on b74 b35)
            (on b75 b60)
            (on b76 b43)
            (on b77 b78)
            (on b78 b91)
            (on b79 b48)
            (on b80 b75)
            (on-table b81)
            (on b82 b46)
            (on b83 b24)
            (on b84 b29)
            (on b85 b26)
            (on b86 b19)
            (on b87 b74)
            (on-table b88)
            (on b89 b27)
            (on b90 b28)
            (on b91 b65)
            (on b92 b21)
            (on b93 b84)
            (on b94 b53)
            (on b95 b71)
            (on-table b96)
            (on b97 b66)
        )
    )
)