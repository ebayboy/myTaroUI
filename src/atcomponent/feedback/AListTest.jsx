import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'
import { AtList, AtListItem } from 'taro-ui'

import 'taro-ui/dist/style/index.scss'

//import './AtTest.scssList';


const LIST = [
    {
        title: 'item11'
    },
    {
        title: 'item12'
    },
    {
        title: 'item13'
    }
]
export default class AtListTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { list: LIST }
    }

    render() {
        const list = this.state.list

        return (
            <View>
                <AtList>
                    {list.map((item, index) => (
                        <AtListItem title={item.title} />
                    ))}
                </AtList>
            </View>
        )
    }
}