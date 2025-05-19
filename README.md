# IoT25-HW07

# Distance Estimation Model
distance(m) = 10 ^ (((-59)-RSSI)/(10*2.0))

// RSSI = Received signal strength (dBm)
// -59 = RSSI value measured at a distance of 1 meter (dBm)
// 2.0 = Path loss exponent

# Test and Analyze   
![output (1)](https://github.com/user-attachments/assets/6c77e024-0fc7-4822-89fd-9e213e4938a5)

# Why there is difference with model?
  1. **Environmental Factors**

    a. Multipath Propagation  
    b. Signal Absorption  
    c. Signal Interference  
  2. **Model Simplification**

    a. 모델에서 pathLossExponent를 고정된 값(2.0) 으로 설정했지만, 실제 환경에서는 1.6 ~ 3.5 범위에서 다양하게 변할 수 있다.  
  3. **BLE Signal Fluctuation**  

    a. BLE 신호가 시간에 따라 변동이 심하다.  
    b. 따라서 단일 샘플이 아닌, 여러 번 측정한 후 평균 값을 사용하는 방법이 더 정확하다.  
# Demo Video
https://drive.google.com/file/d/1HXvBzpx5t_Fgruiuz1zJ3qvO1O_kR6uc/view?usp=sharing

# Screenshot  

<img width="631" alt="HW07_server" src="https://github.com/user-attachments/assets/74a593c1-8d95-4a73-831e-58ca6f292c67" />  

![HW07_client](https://github.com/user-attachments/assets/38f3035b-0aaa-45e1-ba80-d4cfb80033ee)
