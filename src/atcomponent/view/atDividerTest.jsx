import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtDivider, AtIcon } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

//import './AtButtonTatDividerTest.scss';

export default class AtDividerTest extends Component {
    constructor() {
        super(...arguments)
    }

    render() {
        return (
            <View>
                <Text>atDividerTest</Text>
                <AtDivider content='一般分割线'></AtDivider>
                <AtDivider content='分割线 color-font-line' fontColor='red' lineColor='blue'></AtDivider>
                <AtDivider>
                    <AtIcon value='check-circle'></AtIcon>
                </AtDivider>
            </View>
        )
    }
}