# Welcome to AirSim

AirSim은 Unreal Engine을 기반의 드론, 차량 등을 위한 시뮬레이터이다. 오픈소스, 크로스 플랫폼이며 PX4와 같은 비행제어기와 HIL을 지원하며 현실과 같이 물리, 시각효과를 제공하는 시뮬레이션이다. Unreal plugin으로 개발하면 원하는 Unreal 환경을 추가하면 된다.

AirSim 개발의 목적은 AI 연구를 위한 플랫폼으로 딥러닝, 컴퓨터 비전, reinforcement learning 알고리즘을 가지고 자율운행을 실험할 수 있다. 이런 목적으로 AirSim은 데이터를 추출하기 위한 API를 제공하고 플랫폼에 독립적인 방식으로 기체를 조정한다.

**데모 확인**

AirSim을 이용한 드론

[![AirSim Drone Demo Video](docs/images/demo_video.png)](https://youtu.be/-WfTr1-OBGQ)

AirSim을 이용한 Car

[![AirSim Car Demo Video](docs/images/car_demo_video.png)](https://youtu.be/gnz1X3UNM5Y)


## 새로운 소식

* 이제 AirSim은 [multiple vehicles](docs/multi_vehicle.md)을 위한 전체 기능을 제공한다.
* AirSim 1.2 릴리즈! **이 버전은 API와 settings.json에서 변경** [API Upgrade](docs/upgrade_apis.md)와 [Settings Upgrade](docs/upgrade_settings.md) 문서 참고.
* Unreal Engine 4.18와 Visual Studio 2017로 업그레이드. (참고 : [upgrade instructions](docs/unreal_upgrade.md))

[새로 추가된 기능 목록](docs/whats_new.md)

## 시작하기

### Windows
* [바이너리 다운받기](docs/use_precompiled.md)
* [빌드하기](docs/build_windows.md)

### Linux
* [빌드하기](docs/build_linux.md)

## 사용법

### Mode 선택: Car, Multirotor 나 ComputerVision
기본적으로 AirSim은 Car나 Multirotor 모드 중에서 하나를 선택하라는 창이 나온다. [SimMode setting](docs/settings.md#simmode)을 사용해서 기본 기체를 지정하거나 새로운 [ComputerVision mode](docs/image_apis.md#computer-vision-mode-1)를 사용할 수 있다.

### 수동 조정

다음과 같은 리모트 콘트로(RC)를 가지고 있는 경우 시뮬레이터에서 수동으로 드론을 조정할 수 있다. 차량인 경우에는 화살표 키를 이용해서 수동 조정이 가능하다.

[More details](docs/remote_control.md)

![record screenshot](docs/images/AirSimDroneManual.gif)

![record screenshot](docs/images/AirSimCarManual.gif)


### 프로그램으로 제어

AirSim은 프로그램으로 시뮬레이션의 기체와 상호동작시킬 수 있는 API를 제공한다. 이 API를 이용해서 image, 상태 추출 및 기체 제어 등이 가능하다. API는 RPC를 통해 제공되고 다양한 언어를 통해 접근이 가능하다. (C++, Python, C#, Java 등)

이 API는 별도의 독립 크로스-플랫폼 라이브러리의 일부로 가능하므로 기체에 있는 컴패니온 컴퓨터에 넣을 수 있다. 이런 방식으로 시뮬레이터에 코드를 작성하고 테스트할 수 있고 나중에 실제 기체에서 실행할 수 있다. 학습을 전이시키고 관련된 연구가 집중하고 있는 분야 중에 하나다.

[More details](docs/apis.md)

### training data 수집하기

AirSim에서 deep learning을 위해 training data를 생성할 수 있는 2가지 방식있다. 가장 쉬운 방법은 간단히 오른쪽 아래에 있는 record 버튼을 누르면 된다. 각 프레임에 대해서 pose와 image를 기록하기 시작한다. data logging 코드는 아주 단순해서 원하는 방식으로 수정이 가능하다.

![record screenshot](docs/images/record_data.png)

원하는대로 training data를 생성하는 더 나은 방식은 API를 이용하는 것이다. 이렇게 하면 어떻게, 무엇을, 언제, 어디로 log data를 기록할지를 선택할 수 있다.

### Computer Vision 모드

AirSim을 사용하는 다른 방식은 소위 "Computer Vision" 모드이다. 이 모드에서 비행체나 실물을 가지지 않는다. 키보드로 화면 내부에서 움직이거나 API를 사용해서 카메라를 임의 자세로 위치시킬 수 있고 depth, disparity, surface normals, object segmentation와 같이 image를 수집할 수 있다.

[More details](docs/image_apis.md)

## Tutorials

- [Video - Setting up AirSim with Pixhawk Tutorial](https://youtu.be/1oY8Qu5maQQ) by Chris Lovett
- [Video - Using AirSim with Pixhawk Tutorial](https://youtu.be/HNWdYrtw3f0) by Chris Lovett
- [Video - Using off-the-self environments with AirSim](https://www.youtube.com/watch?v=y09VbdQWvQY) by Jim Piavis
- [Reinforcement Learning with AirSim](docs/reinforcement_learning.md) by Ashish Kapoor
- [The Autonomous Driving Cookbook](https://aka.ms/AutonomousDrivingCookbook) by Microsoft Deep Learning and Robotics Garage Chapter
- [Using TensorFlow for simple collision avoidance](https://github.com/simondlevy/AirSimTensorFlow) by Simon Levy and WLU team

## Participate

### Paper

기술적 상세 내용 : [AirSim paper (FSR 2017 Conference)](https://arxiv.org/abs/1705.05065). Please cite this as:
```
@inproceedings{airsim2017fsr,
  author = {Shital Shah and Debadeepta Dey and Chris Lovett and Ashish Kapoor},
  title = {AirSim: High-Fidelity Visual and Physical Simulation for Autonomous Vehicles},
  year = {2017},
  booktitle = {Field and Service Robotics},
  eprint = {arXiv:1705.05065},
  url = {https://arxiv.org/abs/1705.05065}
}
```

### Contribute

프로젝트에 기여하는 방법을 찾는다면 [open issues](https://github.com/microsoft/airsim/issues)을 살펴보자.

* [More on AirSim design](docs/design.md)
* [More on code structure](docs/code_structure.md)
* [Contribution Guidelines](docs/contributing.md)


### AirSim를 사용하는 곳

We are [maintaining a list](docs/who_is_using.md) of a few projects, people and groups that we are aware of. If you would like to be featured in this list please [add a request here](https://github.com/microsoft/airsim/issues).

## Contact

Join the [AirSim group at Facebook](https://www.facebook.com/groups/1225832467530667/) to stay up to date or ask any questions.

## FAQ

문제가 발생하면, 먼저 [FAQ](docs/faq.md)를 확인하고 [AirSim github](https://github.com/Microsoft/AirSim/issues)에 편하게 이슈를 등록하라.

## License

이 프로젝트는 MIT 라이센스를 따른다. 자세한 내용은 [License file](LICENSE)을 참고하라.
