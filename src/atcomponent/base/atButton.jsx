import Taro, { Component } from '@tarojs/taro'
import { View,Text } from '@tarojs/components'

import { AtButton } from 'taro-ui';
export default class AtButtonTest extends Component {
    constructor() {
        super(...arguments)
    }

    render() {
        return (
            <View>
                <Text>AtButtonTest</Text>
                <AtButton>normal</AtButton>
                <AtButton type='primary'>primary</AtButton>
                <AtButton type='secondary' >secondary</AtButton>

                <AtButton type='primary' size='small'>smarll-size</AtButton>
                <AtButton type='primary' size='normal' >normal-size</AtButton>

                <AtButton type='primary' loading >loading</AtButton>
            </View>
        )
    }
}