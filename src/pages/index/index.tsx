import Taro, { Component, Config } from "@tarojs/taro";
import { View } from "@tarojs/components";

import "./index.scss";
import AtIconTest from "../../atcomponent/base/atIconTest";
import AtButtonTest from "../../atcomponent/base/atButton";
import AtFabTest from "../../atcomponent/base/atFabTest";
import AtAvatarTest from "../../atcomponent/view/atAvatarTest";
import ArticleTest from "../../atcomponent/view/articleTest";
import AtBadgeTest from "../../atcomponent/view/atBadgeTest";
import AtCountDownTest from "../../atcomponent/view/atCountdownTest";
import AtCurtainTest from "../../atcomponent/view/atCurtainTest";
import AtLoadMoreTest from "../../atcomponent/view/atLoadMoreTest";
import AtNoticebarTest from "../../atcomponent/view/atNoticebarTest";
import AtTagTest from "../../atcomponent/view/atTagTest";
import AtTimelineTest from "../../atcomponent/view/atTimelineTest";
import SwiperTest from "../../atcomponent/view/swipterTest";
import AtDividerTest from "../../atcomponent/view/atDividerTest";
import AtStepsTest from "../../atcomponent/view/atStepsTest";
import AtActionSheetTest from "../../atcomponent/feedback/atActionSheetTest";
import AtActivityIndicatorTest from "../../atcomponent/feedback/atActivityIndicatorTest";
import AtModalTest from "../../atcomponent/feedback/atModalTest";
import AtProgressTest from "../../atcomponent/feedback/atProgressTest";
import AtToastTest from "../../atcomponent/feedback/atToastTest";
import AtSwipeActionTest from "../../atcomponent/feedback/atSwipeActionTest";
import AtListTest from "../../atcomponent/feedback/atListTest";
import AtMessageTest from "../../atcomponent/feedback/atMessageTest";
import AtFormTest from "../../atcomponent/form/atFormTest";
import AtInputTest from "../../atcomponent/form/atInputTest";
import AtInputNumberTest from "../../atcomponent/form/atInputNumberTest";
import AtRadioTest from "../../atcomponent/form/atRadioTest";
import AtCheckboxTest from "../../atcomponent/form/atCheckboxTest";
import AtRateTest from "../../atcomponent/form/atRateTest";
import AtSwitchTest from "../../atcomponent/form/atSwitchTest";
import AtTextareaTest from "../../atcomponent/form/atTextareaTest";
import PickerTest from "../../atcomponent/form/pickerTest";
import AtSearchBarTest from "../../atcomponent/form/atSearchBarTest";
import AtSliderTest from "../../atcomponent/form/atSliderTest";
import AtImagePickerTest from "../../atcomponent/form/atImagePickerTest";
import AtRangeTest from "../../atcomponent/form/atRangeTest";
import AtViewCSS from "../../atcomponent/layout/flex";
import AtGridTest from "../../atcomponent/layout/atGridTest";
import AtListTestNew from "../../atcomponent/layout/atListTestNew";
import AtCardTest from "../../atcomponent/layout/atCardTest";
import AtFloatLayoutTest from "../../atcomponent/layout/atFloatLayoutTest";
import AtAccordionTest from "../../atcomponent/layout/atAccordionTest";
import AtNavBarTest from "../../atcomponent/navigation/atNavBarTest";
import AtTabBarTest from "../../atcomponent/navigation/AtTabBarTest";
import AtTabsTest from "../../atcomponent/navigation/atTabsTest";
import AtSegmentedControlTest from "../../atcomponent/navigation/AtSegmentedControlTest";
import AtPaginationTest from "../../atcomponent/navigation/atPaginationTest";
import AtDrawerTest from "../../atcomponent/navigation/atDrawerTest";
import AtIndexesTest from "../../atcomponent/navigation/atIndexesTest";
import AtCalendarTest from "../../atcomponent/advance/atCalendarTest";

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

  /*
  onScrollToUpper() {}
  onScroll(e) {
    console.log(e.detail);
  }
  */

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
        <ArticleTest></ArticleTest>
        <AtBadgeTest></AtBadgeTest>
        <AtCountDownTest></AtCountDownTest>
        <AtCurtainTest></AtCurtainTest>
        <AtLoadMoreTest></AtLoadMoreTest>
        <AtNoticebarTest></AtNoticebarTest>
        <AtTagTest></AtTagTest>
        <AtTimelineTest></AtTimelineTest>
        <SwiperTest></SwiperTest>
        <AtDividerTest></AtDividerTest>
        <AtStepsTest></AtStepsTest>
        <AtActionSheetTest></AtActionSheetTest>
        <AtActivityIndicatorTest></AtActivityIndicatorTest>
        <AtModalTest></AtModalTest>
        <AtProgressTest></AtProgressTest>
        <AtToastTest></AtToastTest>
        <AtSwipeActionTest></AtSwipeActionTest>
        <AtListTest></AtListTest>
        <AtMessageTest></AtMessageTest>
        <AtFormTest></AtFormTest>
        <AtInputTest></AtInputTest>
        <AtInputNumberTest></AtInputNumberTest>
        <AtRadioTest></AtRadioTest>
        <AtCheckboxTest></AtCheckboxTest>
        <AtRateTest></AtRateTest>
        <AtSwitchTest></AtSwitchTest>
        <AtTextareaTest></AtTextareaTest>
        <PickerTest></PickerTest>
        <AtSearchBarTest></AtSearchBarTest>
        <AtSliderTest></AtSliderTest>
        <AtImagePickerTest></AtImagePickerTest>
        <AtRangeTest></AtRangeTest>
        <AtViewCSS></AtViewCSS>
        <AtGridTest></AtGridTest>
        <AtListTestNew></AtListTestNew>
        <AtCardTest></AtCardTest>
        <AtFloatLayoutTest></AtFloatLayoutTest>
        <AtAccordionTest></AtAccordionTest>
        <AtNavBarTest></AtNavBarTest>
        <AtTabBarTest></AtTabBarTest>
        <AtTabsTest></AtTabsTest>
        <AtSegmentedControlTest></AtSegmentedControlTest>
        <AtPaginationTest></AtPaginationTest>
        <AtDrawerTest></AtDrawerTest>
        <AtIndexesTest></AtIndexesTest>
        <AtCalendarTest></AtCalendarTest>
      </View>
    );
  }
}
