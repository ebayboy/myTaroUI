
import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'

import { AtBadge, AtButton } from 'taro-ui';

import "./atBadgeTest.scss"

export default class AtBadgeTest extends Component {
    constructor() {
        super(...arguments)
    }

    render() {
        return (
            <View>
                <Text>AtBadgeTest</Text>
                <AtBadge value={10} maxValue={99}>
                    <AtButton size='small'>按钮</AtButton>
                </AtBadge>
                <AtBadge dot>
                    <AtButton size='small'>按钮</AtButton>
                </AtBadge>
                <AtBadge value='NEW'>
                    <AtButton size='small'>按钮</AtButton>
                </AtBadge>
                <AtBadge value={'···'}>
                    <AtButton size='small'>按钮</AtButton>
                </AtBadge>
            </View>
        )
    }
}