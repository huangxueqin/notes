1. pipelined reliable data transmission protocol.  
There are two method using for error recovery in pipelined protocol: `Go-Back-N` and `selective repeat`
    - Go-Back-N(GBN): GBN are also called sliding window protocol.
        - base: minimum seq_num which has been sent, but without ack received.
        - nextseqnum: minimum unused seq_num
        - N: the size of the sending sliding window
        - [0 - base-1]: seq_nums that are sent and acked
        - [base - nextseqnum-1]: seq_nums that are sent, but without ack received
        - [nextseqnum - base+N-1]: seq_nums that can be used by next sent_packet
        - [base+N - ~]: unavailable seq_nums
        - GBN's receiving end uses a method called **cumulative acknowledgment**. Receiving end keeps a variable called expectedseqnum which means
        next seq_num the receiving end expects to receive. When a comming packet has the seq_num equals with expectedseqnum,
        receiving end unpacket the received packet and send it to upper layer. And send a ack packet to sending end with seq_num
        expectedseqnum. Then receiving end add expectedseqnum by 1, continue to wait new packet with seq_num equals with the updated
        expectedseqnum. When the received packet doesn't has expected seq_num, then the receiving end just drop that packet. This
        mechanism make sure that when sending end receives a ack packet with seq_num `t`, the sending end can sure that any packet
        with seq_num less or equal than `t` are correctly received by receiving end. This is what **cumulative acknowledgment** means.
        - GBN's sending end keeps one clock. The clock are used for computing timeout event. As long as sending end send a packet, it
        reset the clock. When timeout happened, the sending end resends all packet with seq_num ranges from base to nextseqnum-1. When
        sending end receives a ack packet with seq_num `t`, it first, adjust base to `t+1`. when the adjusted base equals with nextseqnum
        sending end stop the clock, otherwise, it reset the clock(because there still sending packet that not acked). The window size N
        are used when the sending end receives data from upper layer. if `nextseqnum < base + N`, then sending it, otherwise sending end
        refuse to send the data until nextseqnum becomes less that `base + N` in some time.
        - GBN dosen't perfectly solve the low ultilization of the channel. Because one packet's errornous can make massive resending happens.

   - selective repeat: The difference between GBN and selective repeat is the receiving end of SR caches received packet rather uses cumulative
   acknowledgment. And so, the sending end must keeps each packet a timer for timeout resending.
        - one more important restriction for SR protocol is the length of seq_num space must twice or larger than sending the sliding window size.

2. TCP, TCP also use **cumulative acknowledgment** but combined with **selective repeat**, and Recommending timer mechanism is single timer. Another
important point should know is: The TCP connection is a full duplex connection, so both client and server can be sending end and receiving end.
    - Structure of TCP packet,
        - RTT estimation and timeout management
        - EstimatedRTT = (1-a) * EstimatedRTT + a * SampleRTT
        - DevRTT = (1-b) * DevRTT + b * | SampleRTT - EstimatedRTT|
        - TimeoutInterval = EstimatedRTT + 4 * DevRTT
        - The sending end of TCP

                next_seq_num = init_seq_num;
                send_base = init_send_base;
                while(event) {
                    switch(event) {
                        case Send_Data:
                            if(next_seq_num >= send_base + N) {
                                // refuse to send
                            }
                            else {
                                packet = make_packet(next_seq_num, data);
                                next_seq_num += data.length;
                                send(packet);
                                if(timer is not running) {
                                    start_timer;
                                }
                            }
                            break;
                        case timeout:
                            packet = make_packet(send_base, base_data);
                            send(packet);
                            // in some common implementation, the timeout interval may
                            // set to twice of prior interval
                            start_timer;
                            break;
                        case acknowledgment received:
                            // let acknowledged client seq_num is y
                            if(y > send_base) {
                                send_base = y;
                                start_timer;
                            }
                            else {
                                // record the times of this seq_num y as county;
                                county++;
                                if(county == 3) {
                                    // fast retransmit happens
                                    packet = make_packet(y, y_data);
                                    send(packet);
                                }
                            }
                            break;
                    }
                }

    - flow control service:
        - TCP provides cache mechanism, the receiving keep a buffer to cache the incoming packet. There are three variables the receiving end used to track the state of receiving buffer: last_byte_received(the last byte number that received), last_byte_readed(the last byte number that upper application readed) and recv_buffer_size. So the empty place of the receiving buffer named `recv_window = recv_buffer_size - (last_byte_received - last_byte_readed)` and TCP must make sure that recv_window should larger than 0. At the initial time, receiving end make `recv_window = recv_buffer_size` and each time sending ACK back, the `recv_window` is filled in the `Window` field in TCP header.
        - In sending end, there also two variables are kept: last_byte_send and last_byte_acked. The sending end must keep `last_byte_send - last_byte_acked <= recv_window` in the whole life cycle. So the sending end code above should be modified for flow control.
        - There also a special situation when the recv_window equals 0. In such situation, according the previous description, the sending end will
        never send any packet and never receive any ACK packet as well. This leads the connection can't transmit any data. To solve this dilemma,
        TCP protocol claims that in such situation, the sending end should continue to send packet with only one byte until the cache in receiving
        end cleared by upper application so that the normal transmit state recovered.
    - connection management
        - connection establishment: The connection establishment are also called `three handshakes`. The process is described here.  
            - Client send an special packet called SYN packet by make the SYN bit in TCP header set. And the sequence number part are filled by a carefully choosed number `client_isn`.
            - The server are listening each connection request using `listen` function. When receiving a valid SYN packet, it response the request by
            sending back a SYN packet which has sequence number part filled by a carefully choosed number `server_isn` and has the acknowledgment number
            part filled with `client_isn` which extract from the received SYN packet add by 1.
            - Client receives the acknowledgment SYN packet. This time it send a packet with SYN bit unset. And the sequence number part filled with
            `client_isn+1`, the acknowledgment filled with `server_isn + 1`. This time the packet can also brings data to transmit. After these three
            step, the connection are established.
        - shutdown process: When client shutdown the connection, following shows the states of client part and server part
            - client: CLOSED --(SYN sended)-->  SYN_SEND --(receive syn & ack, send ack)--> ESTABLISHED --(send FIN)--> FIN_WAIT_1
              --(receive ack)--> FIN_WAIT_2 --(receive FIN send ack)--> TIME_WAIT --(wait for 30s)--> CLOSED
            - server: CLOSED --> LISTEN --(receive syn send syn&ack)--> SYN_RECV --(receive ack)--> ESTABLISHED --(receive FIN send ack)-->
              CLOSE_WAIT --(send FIN)--> LAST_ACK --(receive ack)--> CLOSED

    - congestion control: TCP use end-to-end congestion control. Why data packet disappear in transmition? One reason is when router's cache full,
    the router will drop the newly incoming data packet.
