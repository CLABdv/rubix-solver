module Main where

data Colour = White | Blue | Orange | Yellow | Green | Red
    deriving (Eq, Show, Ord)

preprocess :: String -> ([Double], Double, Colour)
preprocess x = undefined
    where (p1, p2) = let i = length $ takeWhile (/=']') x in splitAt (i+1) x
          dir = read p1 :: [Double]
          (len, label) = let [a, b] = words $ filter (/=',') p2 in (read a :: Double, f b)
          f y = case y of
            "white" -> White
            "blue" -> Blue
            "orange" -> Orange
            "yellow" -> Yellow
            "green" -> Green
            "red" -> Red


main :: IO ()
main = putStrLn "Hello, Haskell!"
