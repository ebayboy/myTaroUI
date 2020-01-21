import Taro, { Component, Config } from "@tarojs/taro";
import { View } from "@tarojs/components";

import { AtButton, AtActionSheet, AtActionSheetItem } from "taro-ui";

import "./index.scss";

export default class Index extends Component {
  /**
   * 指定config的类型声明为: Taro.Config
   *
   * 由于 typescript 对于 object 类型推导只能推出 Key 的基本类型
   * 对于像 navigationBarTextStyle: 'black' 这样的推导出的类型是 string
   * 提示和声明 navigationBarTextStyle: 'black' | 'white' 类型冲突, 需要显示声明类型
   */
  config: Config = {
    navigationBarTitleText: "Taro"
  };

  constructor() {
    super(...arguments);

    this.state = {
      isOpened: false
    };
  }

  componentWillMount() {
    let myName = "Tom";
    console.log(`My name is ${myName}`);
    console.log("willMount isOpened value:", this.state.isOpened);
  }

  componentDidMount() {}

  componentWillUnmount() {}

  componentDidShow() {}

  componentDidHide() {}

  handleClick = () => {
    this.setState({ isOpened: true });
  };

  handleClose = () => {
    console.log("handleClose isOpened value:", this.state.isOpened);
  };

  showToast(name) {
    Taro.showToast({
      icon: "none",
      title: name
    });
  }

  handleClickAction(type) {
    console.log("clickAction :", type);
    console.log("clickAction after isOpened value:", this.state.isOpened);
  }

  render() {
    console.log("render isOpened value:", this.state.isOpened)
    const isOpened = this.state.isOpened;

    return (

      <View>
        <View>
          <AtButton onClick={this.handleClick}>打开 ActionSheet</AtButton>
        </View>
             <AtActionSheet isOpened = {isOpened}
                    cancelText='取消'
                    title='头部标题可以用通过转义字符换行'
                    onClose={this.handleClose}
                >
                    <AtActionSheetItem
                        onClick={this.handleClickAction.bind(this, '点击了按钮一')}
                    >
                        按钮一
                    </AtActionSheetItem>
                    <AtActionSheetItem
                        onClick={this.handleClickAction.bind(this, '点击了按钮二')}
                    >
                        按钮二
                    </AtActionSheetItem>
                </AtActionSheet>
            </View>
  }
}
