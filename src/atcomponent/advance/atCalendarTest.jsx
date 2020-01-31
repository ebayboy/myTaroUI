import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtCalendar } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtCalendarTest extends Component {
    constructor() {
        super(...arguments)
    }

    onDayClick(e) {
        console.log("e:", e)
    }

    render() {
        return (
            <View>
                <AtCalendar marks={[{ value: '2020/01/30' }]}
                    format='YYYY-MM-DD'
                    onDayClick={this.onDayClick.bind(this)}
                />
            </View>
        )
    }
}
