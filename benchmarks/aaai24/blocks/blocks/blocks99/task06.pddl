(define (problem BW-99-1-6)
    (:domain blocksworld)
    (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31 b32 b33 b34 b35 b36 b37 b38 b39 b40 b41 b42 b43 b44 b45 b46 b47 b48 b49 b50 b51 b52 b53 b54 b55 b56 b57 b58 b59 b60 b61 b62 b63 b64 b65 b66 b67 b68 b69 b70 b71 b72 b73 b74 b75 b76 b77 b78 b79 b80 b81 b82 b83 b84 b85 b86 b87 b88 b89 b90 b91 b92 b93 b94 b95 b96 b97 b98 b99 - block)
    (:init
        (handempty)
        (on b1 b24)
        (on b2 b13)
        (on b3 b41)
        (on b4 b10)
        (on b5 b15)
        (on-table b6)
        (on b7 b55)
        (on b8 b33)
        (on b9 b14)
        (on b10 b49)
        (on b11 b60)
        (on b12 b6)
        (on b13 b30)
        (on b14 b64)
        (on b15 b65)
        (on b16 b77)
        (on b17 b11)
        (on b18 b4)
        (on b19 b62)
        (on b20 b50)
        (on b21 b71)
        (on b22 b90)
        (on b23 b43)
        (on b24 b88)
        (on b25 b27)
        (on b26 b36)
        (on-table b27)
        (on b28 b91)
        (on b29 b86)
        (on b30 b99)
        (on b31 b83)
        (on b32 b97)
        (on b33 b59)
        (on b34 b16)
        (on b35 b69)
        (on b36 b73)
        (on b37 b19)
        (on b38 b58)
        (on b39 b9)
        (on b40 b17)
        (on b41 b63)
        (on b42 b56)
        (on b43 b85)
        (on b44 b98)
        (on b45 b38)
        (on b46 b12)
        (on b47 b87)
        (on b48 b92)
        (on b49 b44)
        (on b50 b34)
        (on b51 b1)
        (on-table b52)
        (on b53 b95)
        (on b54 b8)
        (on b55 b20)
        (on b56 b28)
        (on b57 b26)
        (on b58 b94)
        (on b59 b52)
        (on b60 b3)
        (on b61 b35)
        (on b62 b51)
        (on b63 b47)
        (on-table b64)
        (on b65 b40)
        (on b66 b7)
        (on b67 b93)
        (on b68 b66)
        (on b69 b80)
        (on b70 b32)
        (on b71 b72)
        (on b72 b78)
        (on b73 b84)
        (on b74 b76)
        (on b75 b89)
        (on b76 b29)
        (on b77 b46)
        (on b78 b48)
        (on b79 b45)
        (on b80 b5)
        (on b81 b96)
        (on b82 b75)
        (on b83 b18)
        (on b84 b42)
        (on b85 b79)
        (on-table b86)
        (on b87 b25)
        (on b88 b53)
        (on b89 b22)
        (on b90 b81)
        (on b91 b74)
        (on b92 b67)
        (on b93 b82)
        (on-table b94)
        (on b95 b23)
        (on b96 b39)
        (on b97 b54)
        (on-table b98)
        (on b99 b61)
        (clear b2)
        (clear b21)
        (clear b31)
        (clear b37)
        (clear b57)
        (clear b68)
        (clear b70)
    )
    (:goal
        (and
            (on b1 b7)
            (on b2 b50)
            (on b3 b99)
            (on-table b4)
            (on b5 b79)
            (on b6 b24)
            (on b7 b8)
            (on b8 b77)
            (on b9 b42)
            (on b10 b14)
            (on b11 b72)
            (on b12 b2)
            (on b13 b81)
            (on b14 b52)
            (on b15 b54)
            (on b16 b71)
            (on b17 b19)
            (on b18 b44)
            (on b19 b39)
            (on b20 b31)
            (on b21 b55)
            (on b22 b15)
            (on-table b23)
            (on b24 b40)
            (on b25 b23)
            (on b26 b45)
            (on b27 b94)
            (on b28 b62)
            (on b29 b84)
            (on b30 b13)
            (on b31 b34)
            (on b32 b35)
            (on b33 b5)
            (on b34 b17)
            (on-table b35)
            (on b36 b67)
            (on b37 b11)
            (on b38 b85)
            (on b39 b30)
            (on b40 b12)
            (on b41 b89)
            (on b42 b59)
            (on b43 b22)
            (on b44 b57)
            (on b45 b38)
            (on b46 b16)
            (on b47 b93)
            (on-table b48)
            (on b49 b3)
            (on b50 b1)
            (on b51 b46)
            (on b52 b60)
            (on b53 b27)
            (on b54 b92)
            (on b55 b32)
            (on b56 b4)
            (on b57 b73)
            (on b58 b78)
            (on b59 b26)
            (on-table b60)
            (on b61 b82)
            (on b62 b76)
            (on b63 b97)
            (on b64 b66)
            (on b65 b9)
            (on-table b66)
            (on-table b67)
            (on b68 b65)
            (on b69 b95)
            (on-table b70)
            (on b71 b43)
            (on b72 b47)
            (on b73 b33)
            (on b74 b83)
            (on b75 b91)
            (on b76 b21)
            (on b77 b86)
            (on b78 b68)
            (on b79 b28)
            (on b80 b20)
            (on-table b81)
            (on b82 b70)
            (on b83 b69)
            (on b84 b58)
            (on b85 b88)
            (on b86 b37)
            (on-table b87)
            (on b88 b51)
            (on b89 b63)
            (on b90 b25)
            (on b91 b90)
            (on b92 b96)
            (on b93 b18)
            (on b94 b87)
            (on b95 b98)
            (on b96 b75)
            (on b97 b56)
            (on b98 b49)
            (on b99 b61)
        )
    )
)