import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtCountdown } from 'taro-ui'

import './atCountDownTest.scss';

export default class AtCountDownTest extends Component {
    constructor() {
        super(...arguments)
    }

    onTimeUp() {
        Taro.showToast({
            title: '时间到',
            icon: 'success',
            duration: 2000
        })
    }

    render() {
        return (
            <View>
                <Text>AtCountDownTest</Text>
                <AtCountdown
                    format={{ hours: ':', minutes: ':', seconds: '' }}
                    seconds={10}
                    onTimeUp={this.onTimeUp.bind(this)}
                />
                <AtCountdown
                    isShowDay
                    day={2}
                    hours={1}
                    minutes={1}
                    seconds={10}
                />
                <AtCountdown
                    isShowDay
                    format={{ hours: ':', minutes: ':', seconds: '' }}
                    day={2}
                    hours={1}
                    minutes={1}
                    seconds={10}
                />
                <AtCountdown
                    isCard
                    minutes={1}
                    seconds={10}
                />
            </View>
        )
    }
}