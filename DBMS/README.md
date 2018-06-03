> Effeiciency of Buffer Management system based on Buffer number 
----------

![disk_based](/uploads/65a37e097c04e64e1914a013c9c4d5a0/disk_based.PNG)
`1.버퍼를 사용하지 않고 disk based로 코드를 돌렸을 때(2^20 개 record) 진행 속도`

![16](/uploads/621b420eb0cee96619aa71d79dc5c6f5/16.PNG)
`2.버퍼의 크기를 16으로 잡아주고  코드를 돌렸을 때(2^20 개 record) 진행 속도`

![160](/uploads/5a773aaf9d4c0f10926c9e8707922ecd/160.PNG)
`3.버퍼의 크기를 160으로 잡아주고 코드를 돌렸을때(2^20 개 record) 진행 속도`

![1600](/uploads/92e08b3b1b178f1d1107d6c7bb5eedbf/1600.PNG)
`4.버퍼의 크기를 1600으로 잡아주고 코드를 돌렸을 때(2^20 개 record) 진행 속도`

작성한 코드에서 버퍼의 갯수를 다음과 같이 잡아 주고, 각각 2^20 개의 record를 sequential, random 하게 insert, delete 해 준 결과(총 4가지 방법) 다음과 같은 시간이 소모 되었다.


이를 서로 비교하기 편하게 그래프로 표현해 보면
![그래프](/uploads/9f9da49b4317c5b1687b96a189041a5c/그래프.PNG)
다음과 같은 그래프의 모양을 보이는데, 버퍼를 사용할 때와 사용하지 않았을 때의 실행속도에 있어 확연한 차이를 느낄 수 있다.
하지만, 버퍼가 160개까지는 버퍼가 늘어날 수록 실행속도가 대체적으로 줄어드는 반면, 1600개 일때는 오히려 조금 속도가 늘어난 것을 확인 할 수 있는데, 이는
![image](/uploads/e935afa006319f038227c0542696f1fd/image.png)
다음과 같이, 함수에서 buffer에서 page를 읽어오거나, 디스크에서 buffer에 page를 올리려고하는 순간에, for문을 통해 버퍼 전체를 sequential하게 searching해주기 때문에, buffer의 갯수가 급격하게 많아지면 오히려 실행속도가 좀 더 늘어나는 것이라고 판단된다.
이를 해결하기 위해, hash table을 작성하는 방법도 생각해봤지만, 과제 기한에 맞추어 코드를 작성하다 보니 그 기능까지는 디자인하지 못하였다.


