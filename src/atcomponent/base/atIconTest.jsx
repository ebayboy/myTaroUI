import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'

import { AtIcon } from 'taro-ui';
export default class AtIconTest extends Component {
    constructor() {
        super(...arguments)
    }

    render() {
        return (
            <View>
                <Text>AtIconTest</Text>
                <AtIcon value='clock' size='30' color='#F00'></AtIcon>
            </View>
        )
    }
}