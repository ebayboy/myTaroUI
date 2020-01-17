import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'

import { AtAvatar } from 'taro-ui';
export default class AtAvatarTest extends Component {
    constructor() {
        super(...arguments)
    }

    render() {
        return (
            <View>
                <Text>AtAvatarTest</Text>
                <AtAvatar image='https://jdc.jd.com/img/200'></AtAvatar>
                <AtAvatar text='凹凸实验室'></AtAvatar>
                <AtAvatar circle image='https://jdc.jd.com/img/200'></AtAvatar>
                <AtAvatar circle text='凹凸实验室'></AtAvatar>
            </View>
        )
    }
}