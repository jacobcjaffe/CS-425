Commits \
1: created a bash file to quickly automate testing \
2: utilized the getNext function to retrieve the next number. \
3: created two iterators. Have half of the threads getNext from the front, and \
    the other half retrieving from the back of the numbers. Seemed to be my best time... \
4: Tried creating a queue, but timing is a lot worse. I think it may have to do with copying the \
    data repeatedly 
5: Simply used the getNext using a vector, pulling 6 at a time. \
6: Pulled from the vector 80 at a time. 

# These were from using test file
Inital Timing | 2m45.200s |
------------- | --------- |
2 iterators   | 1m21.726s |
getNext()     | 1m47.219s |
getNext 6     | 1m41.608s |
getNext 80    | 1m26.126s |
