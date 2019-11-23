-- Running instructions 
-- gchi ass1.hs
-- main

tree :: Integer -> [String]

tree n = 
    if n == 1
        then ["(.)"]
        else 
            map (\t -> "(" ++ t ++ ".)") (tree (n-1)) ++ map (\t -> "(." ++ t ++ ")") (tree (n-1))


main = do
    putStrLn "Enter the number of nodes: "
    n <- getLine
    let num = (read n :: Integer)
    mapM_ putStrLn (tree num)

