import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'
import { AtMessage, AtButton } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

//import './AtTest.scssMessage';

export default class AtMessageTest extends Component {
    constructor() {
        super(...arguments)
    }

    handleClick(type) {
        Taro.atMessage({
            message: '消息通知',
            type: type,
            duration: 3000
        })
    }

    render() {
        return (
            <View>
                <AtMessage />
                <AtButton onClick={this.handleClick.bind(this, '')}>普通消息</AtButton>
                <AtButton onClick={this.handleClick.bind(this, 'success')}>成功消息</AtButton>
                <AtButton onClick={this.handleClick.bind(this, 'error')}>错误消息</AtButton>
                <AtButton onClick={this.handleClick.bind(this, 'warning')}>警告消息</AtButton>
            </View>
        )
    }
}