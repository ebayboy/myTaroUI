import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtActivityIndicator } from 'taro-ui'

import "taro-ui/dist/style/components/activity-indicator.scss"
import 'taro-ui/dist/style/components/loading.scss'

export default class ActivityIndicatorTest extends Component {
    constructor() {
        super(...arguments)
    }

    render() {
        return (
            <View>
                <Text>AtActivityIndicatorTest</Text>
                <AtActivityIndicator color='#13CE66'></AtActivityIndicator>
            </View>
        )
    }
}