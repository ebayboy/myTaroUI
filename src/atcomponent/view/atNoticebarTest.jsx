import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'

import { AtNoticebar } from 'taro-ui'

import './atNoticebarTest.scss';

export default class AtNoticebarTest extends Component {
    constructor() {
        super(...arguments)
    }

    render() {
        return (
            <View>
                <Text>atNoticebarTest</Text>
                <AtNoticebar>这是基本Notice Bar通告栏</AtNoticebar>
                <AtNoticebar marquee>
                    文字滚动Notice Bar通告栏111, 文字滚动Notice Bar通告栏222, 文字滚动Notice Bar通告栏333
                </AtNoticebar>
                <AtNoticebar icon='volume-plus'>
                    图标Notice Bar1， 图标Notice Bar2， 图标Notice Bar3
                </AtNoticebar>
            </View>
        )
    }
}