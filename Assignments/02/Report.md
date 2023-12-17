Commits \
1: created a bash file to quickly automate testing \
2: Use the getNext function to retrieve the next number. \
3: create two iterators. Have half of the threads getNext from the front, and \
    the other half retrieving from the back of the numbers. Seemed to be my best time... \
4: Tried creating a queue, but the timing is a lot worse. I think it may have to do with copying the \
    data repeatedly 
5: Simply use the getNext using a vector, pulling 6 at a time. \
6: Pulled from the vector 80 at a time. 

# timings with final
Initial Timing | 2m45.200s |
------------- | --------- |
2 iterators   | 1m21.726s |
getNext()     | 1m47.219s |
getNext 6     | 1m41.608s |
getNext 80    | 1m26.126s |
