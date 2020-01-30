import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtIndexes } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtIndexesTest extends Component {
    constructor() {
        super(...arguments)
    }
    onClick(value) {
        console.log("value:", value)
    }

    render() {

    const list = [{
        title: 'A',
        key: 'A',
        items: [
            {
                'name': '阿坝'
                // 此处可加其他业务字段
            },
            {
                'name': '阿拉善'
            }]
    },
    {
        title: 'B',
        key: 'B',
        items: [
            {
                'name': '北京'
            },
            {
                'name': '保定'
            }]
    }
    ]


        return (
            <View style='height:100vh'>
                <AtIndexes
                    list={list}
                    onClick={this.onClick.bind(this)}
                >
                    <View>自定义内容</View>
                </AtIndexes>
            </View>
        )
    }
}