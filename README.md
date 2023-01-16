# Final project report
## 實作
1. ac_trie ← null trie
2. txt_id_2_title_name ← null vector
// txt序號對應標題
3. patterns_2_txt_id ← null map（{string, mode} -> set(txt_ids)）
// query pattern對應txt序號
5. parse query.txt
6. 對query.txt的每一行
    * 對一行中的每個word
        * 將word加進ac_trie中
        // add之餘會額外再terminal node記錄這個word
4. 對ac_trie建fail_pointer
// bfs建法
// 建fail_point時會同時將terminal node所存的strings傳到下個合理的terminal node
// 例如：composition在處理時會被傳到decomposition裡
// 所以decomposition的最後一個n會記錄decomposition, composition
5. 對data dir的每個data.txt
    * parse data.txt
    * 將標題push進txt_id_2_title_name
    * 對標題中的每個word
        * query這個word
        // query同時判斷match到的模式（前綴、exact、後綴）
        // match到將結果（string+模式）insert進patterns_2_txt_id
    * 對data.txt的每一行
        * 對一行中的每個word
            * query這個word
            // query同時判斷match到的模式（前綴、exact、後綴）
            // match到將結果（string+模式）insert進patterns_2_txt_id
8. 第二次parse query.txt
9. 對query.txt的每一行
    * ans ← null set(txt_ids)
    * 對一行中的每個pattern（string+模式）
        * 以patterns_2_txt_id取出他們符合的文章id set
        * 依需求做交集、聯集ans
    * 依ans中的txt_ids與txt_id_2_title_name取出對應的標題，最後輸出
## 遇到的困難
### 在"boyer-moore演算法"與"AC自動機"兩者間舉棋不定
雖然"boyer-moore"是許多瀏覽器ctrl-f的實作方法，但使用情境比較侷限於一次一個query且能online使用；而這次的專案內容是要實現多個query並且算是offline判斷，所以後來決定使用"AC自動機"一次處理所有多個query。
### 後綴模式+前綴模式
一般的AC自動機支援的搜尋模式是：只能判斷match與否。而本次專案所開的spec是要能支援前綴、exact、後綴三種模式。實作上，我在建"AC自動機"時會額外儲存terminal node所代表的strings，在拿txt論文來搜尋的時候，發現match了就會取出此terminal node代表的strings（符合的query strings）。最後依據兩個判斷，第一是"query strings的長度與txt論文中match到的word長度"與第二是"目前match到word的char位置"，來比對判斷此match到的string合格的query模式有哪些。
#### 範例：penapple，長度m為8
1. query = pen前綴，長度n為3。
此模式需要比對n。當match到pen時text會跑到penapple的第n(3)個char，那就是前綴。
3. query = apple後綴，長度n為5。
此模式不需要比對m、n。只需要知道說match到apple時text會跑到penapple的最後一個char，那就是後綴。
5. query = penapple exact，長度n為8。
m與n相等，所以是exact。
### input query處理
延續上述內容，我的寫法在"建AC自動機"上需要關注的資訊僅是每個query的string（能做出trie跟存strings進terminal node），而最後在"製作答案"時才會考慮每個query的string+mode。於是我就設計我的實作方法有2次打開query.txt的部分，第一次在"建AC自動機"時打開，用助教給的word_parser來獲取每個query的string；第二次在最後要"輸出答案"時再打開，並用我自己寫的pattern_parser來獲取每個query的string+mode。
## 參考資料
https://leetcode.com/problems/stream-of-characters/solutions/279084/simple-implementation-of-aho-corasick-algorithm/