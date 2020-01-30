import Taro, { Component } from '@tarojs/taro'
import { View } from '@tarojs/components'

import { AtPagination } from 'taro-ui'
import 'taro-ui/dist/style/index.scss'

export default class AtPaginationTest extends Component {
    constructor() {
        super(...arguments)
        this.state = { current: 1 }
    }

    handleChange(e) {
        console.log("type:", e.type, "current:", e.current)
        this.setState({ current: e.current })
    }

    render() {
        return (
            <View>
                <AtPagination
                    total={50}
                    pageSize={10}
                    current={this.state.current}
                    onPageChange={this.handleChange.bind(this)}
                    icon={true}
                />
            </View>
        )
    }
}