
import Taro, { Component } from '@tarojs/taro'
import { Text, View } from '@tarojs/components'
import { AtTimeline } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

export default class AtTimelineTest extends Component {
    constructor() {
        super(...arguments)
    }

    render() {
        return (
            <View>
                <Text>AtTimelineTest</Text>
                <AtTimeline
                    pending
                    items={[
                        { title: '刷牙洗脸', content: ['大概8:00'], icon: 'check-circle' },
                        { title: '吃早餐', content: ['牛奶+面包', '餐后记得吃药'], icon: 'clock' },
                        { title: '上班', content: ['查看邮件', '写PPT', '发送PPT给领导'], icon: 'clock' },
                        { title: '睡觉', content: ['不超过23:00'], color: 'red' }
                    ]}
                >
                </AtTimeline>
            </View>
        )
    }
}