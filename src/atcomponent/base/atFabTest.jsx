/* 浮动操作按钮（FAB）代表着应用中最重要的操作，是为了使用户更便于操作。
就像一个圆形的图标漂浮在界面上，它会在聚焦时改变颜色，在选中时改变海拔高度。按下时，它可能会包含更多相关的操作。
一个屏幕上建议只使用一个浮动操作按钮，它应该只代表最常见的操作。 */

import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'

import { AtButton, AtFab } from 'taro-ui';
export default class AtFabTest extends Component {
    constructor() {
        super(...arguments)
    }

    render() {
        return (
            <View>
                <Text>AtFabTest</Text>
                <AtFab>
                    <Text className='at-fab__icon at-icon at-icon-menu' ></Text>
                </AtFab>
            </View>
        )
    }
}