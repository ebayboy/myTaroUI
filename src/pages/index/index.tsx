import Taro, { Component, Config } from "@tarojs/taro";
import { Text, View } from "@tarojs/components";

import "./index.scss";
import AtIconTest from "../../atcomponent/base/atIconTest";
import AtButtonTest from "../../atcomponent/base/atButton";
import AtFabTest from "../../atcomponent/base/atFabTest";
import AtAvatarTest from "../../atcomponent/view/atAvatarTest";

export default class Index extends Component {
  /**
   * 指定config的类型声明为: Taro.Config
   *
   * 由于 typescript 对于 object 类型推导只能推出 Key 的基本类型
   * 对于像 navigationBarTextStyle: 'black' 这样的推导出的类型是 string
   * 提示和声明 navigationBarTextStyle: 'black' | 'white' 类型冲突, 需要显示声明类型
   */

  constructor() {
    super(...arguments);
  }

  config: Config = {
    navigationBarTitleText: "首页"
  };

  componentWillMount() {}

  componentDidMount() {}

  componentWillUnmount() {}

  componentDidShow() {}

  componentDidHide() {}

  onScrollToUpper() {}

  // or 使用箭头函数
  // onScrollToUpper = () => {}
  onScroll(e) {
    console.log(e.detail);
  }

  render() {
    /* Swiper最高150， 当超过这个高度使用scrollview进行滚动，以显示全部内容 */
    /*
    const scrollStyle = {
      height: "150px"
    };
    const scrollTop = 0;
    const Threshold = 20;
    const vStyleA = {
      height: "150px",
      "background-color": "rgb(241,241,241)",
      color: "#333"
    };
    */

    return (
      <View className="container">
        <AtButtonTest></AtButtonTest>
        <AtIconTest></AtIconTest>
        <AtFabTest></AtFabTest>
        <AtAvatarTest></AtAvatarTest>
      </View>
    );
  }
}
